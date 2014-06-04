/*
 * Description:   Scheduling functions
 */
#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <schedule.h>
#include <interrupt.h>
#include <queueing.h>
#include <syscalls.h>
#include <cache.h>
#include <config.h>
#include <kernel/arch/special.h>
#include <soc/arch/interface.h>

volatile u64_t scheduler_t::current_time = 0;

CONTINUATION_FUNCTION(idle_thread);

void
scheduler_t::set_active_thread(tcb_t * tcb, tcb_t * schedule)
{
    set_active_schedule(schedule);
}

/**
 * find_next_thread: selects the next tcb to be dispatched
 *
 * returns the selected tcb, if no runnable thread is available,
 * the idle tcb is returned.
 */
prio_t
scheduler_t::get_highest_priority(void)
{
    /* Determine the second-level bitmap to use. */
    word_t first_level_bitmap = prio_queue.index_bitmap;
    if (!first_level_bitmap) {
        return (prio_t)-1;
    }
    word_t first_level_index = msb(first_level_bitmap);

    /* Fetch the second-level bitmap. */
    word_t second_level_bitmap = prio_queue.prio_bitmap[first_level_index];
    word_t second_level_index = msb(second_level_bitmap);
    /* Calculate the top priority. */
    prio_t top_prio = first_level_index * BITS_WORD + second_level_index;

    return top_prio;
}

void
scheduler_t::set_effective_priority(tcb_t *tcb, prio_t prio)
{
    SMT_ASSERT(ALWAYS, schedule_lock.is_locked(true));
    prio_queue.set_effective_priority(tcb, prio);
}

/**
 * Selects the next runnable thread and activates it.  Will insert the
 * current thread into the ready queue if it is not already present.
 *
 * This function updates the scheduled variable in the current tcb to
 * true if the schedule was successful (another thread was switched
 * to) or false if the schedule was unsuccessful.
 *
 * @param current The currently executing tcb
 * @param continuation The continuation to activate upon completion
 *
 * @return true if a runnable thread was found, false otherwise
 */
void
scheduler_t::schedule(tcb_t * current, continuation_t continuation,
                      flags_t flags)
{
    schedule_lock.lock();

    /* No longer violating the scheduler. */
    scheduling_invariants_violated = false;

    /* Determine the next priority thread. */
    prio_t max_prio = get_highest_priority();

    /* If the current thread has an equal priority, we can keep running
     * it. */
    bool current_runnable = current->get_state().is_runnable()
        && !current->is_reserved();
    if (current_runnable) {
        /* If we are the highest priority, or we are not doing a round
         * robin and we are equal highest, just keep running current. */
        if (current->effective_prio > max_prio ||
                (!(flags & sched_round_robin)
                        && current->effective_prio == max_prio)) {
            schedule_lock.unlock();
            ACTIVATE_CONTINUATION(continuation);
        }
        /* If we are pre-empting the thread and the thread has asked for
         * pre-emption notification, inform the thread of this schedule. */
        if (flags & preempting_thread) {
            mark_thread_as_preempted(current);
        }

    }

    /* Otherwise, we are switching to another thread. */
    tcb_t *next;
    if (max_prio >= 0) {
        next = prio_queue.get(max_prio);
        dequeue(next);
    } else {
        next = get_idle_tcb();
    }

    /* Grab the thread. */
    (void)next->grab();
    ASSERT(ALWAYS, next->is_grabbed_by_me());

    /* Switch away from the current thread, enqueuing if necessary. */
    switch_from(current, continuation);
    current->release();
    if (current_runnable && current != get_idle_tcb()) {
        enqueue(current);
    }

    /* Activate the new thread. */
    set_active_thread(next, next);
    schedule_lock.unlock();
    switch_to(next, next);
}

extern "C" void SECTION(".init")
c_schedule(scheduler_t * scheduler, tcb_t * current,
        continuation_t continuation, word_t flags)
{
    scheduler->schedule(current, continuation, flags);
}

/**
 * Expire the given schedule, moving the owner of the schedule to the back of
 * the queue associated with its priority.
 *
 * For SMT systems, scheduler is assumed to be locked before function entry
 */
void
scheduler_t::end_of_timeslice(tcb_t * schedule)
{
    ASSERT(DEBUG, schedule != get_idle_tcb());

    /*
     * Pull the thread to the back of the queue by removing it
     * from the list and appending it to the end. Other threads
     * should remain in the same order.
     *
     * If it is the end of the timeslice for the currently running
     * thread, it is already off the ready queue and will be put
     * at the back when it is enqueued again.
     */
    if (EXPECT_FALSE(schedule != get_current_tcb())) {
        schedule_lock.lock();
        if (schedule->ready_list.is_queued()) {
            dequeue(schedule);
            enqueue(schedule);
        }
        schedule_lock.unlock();
    }

    /* Renew the schedule's timeslice. */
    schedule->current_timeslice = schedule->timeslice_length;
}

/**
 *  Throw away the rest of the current timeslice and yield control to
 *  any other thread with our same priority or higher.
 *
 *  Note that the most likely outcome of this function is that
 *  'current' is scheduled again. The only exception is if there
 *  happens to be another thread at the same priority as 'current'
 *  ready to run.
 *
 *  In a strict-priority scheduler, it is generally wrong to call this
 *  function for any reason other than carrying out a user-space
 *  request to yield, which in turn was probably wrong to ask for a
 *  yield().
 */
void
scheduler_t::yield(tcb_t * current, tcb_t * active_schedule,
        continuation_t continuation)
{
    /* Expire the current schedule's timeslice. */
    end_of_timeslice(get_active_schedule());

    /* Perform a schedule. */
    schedule(current, continuation, scheduler_t::sched_round_robin);
}

/**
 * handle a timer interrupt - update scheduling information
 * and pick a new thread to run according to the scheduling algorithm
 *
 * This is a control function, so it returns by activating the given
 * continuation
 *
 * @param continuation The continuation to activate upon completion
 */
INLINE NORETURN void
scheduler_t::handle_timer_interrupt(bool wakeup, word_t timer_length, continuation_t continuation)
{
}

/**
 * The idle thread merely carrys out a system sleep, normally resulting
 * in the processor halting until the next interrupt arrives.
 *
 * The idle thread should never be executed while other threads in the
 * system are still runnable.
 */
CONTINUATION_FUNCTION(idle_thread)
{
    preempt_enable(idle_thread);
    while (1) {
        processor_sleep();
    }

}

extern word_t pre_tcb_init;

/**
 * initialise all of the kernel threads in the system, along with the
 * roottask  in userland.
 *
 * It also contains the optional startup call to KDB
 *
 * It also flushes the kip from the cache so that there are no cache
 * aliases of the kip to trip up users of virtually addressed cache
 * architectures
 *
 * This is a continuation function but does not use any parameters
 * stored in the TCB
 *
 * This function activates the idle_thread function upon completion
 */
CONTINUATION_FUNCTION(init_all_threads)
{
    pre_tcb_init = 0;
    run_init_script(INIT_PHASE_OTHERS);

    /* Perform the first schedule, and become the idle-thread proper when we
     * once again regain control. */
    get_current_scheduler()->schedule(get_current_tcb(), idle_thread, scheduler_t::sched_default);
}

SYS_THREAD_SWITCH(capid_t dest)
{
    /* Make sure we are in the ready queue to
     * find at least ourself and ensure that the thread
     * is rescheduled */
    tcb_t * current = get_current_tcb();
    scheduler_t * scheduler = get_current_scheduler();
    continuation_t continuation = ASM_CONTINUATION;
    NULL_CHECK(continuation);

    /* Are we performing a simple yield? */
    if (dest.is_nilthread()) {
        /* Yield to another thread running at the same priority. */
        scheduler->yield(current, get_active_schedule(), continuation);
        NOTREACHED();
    }

    /* Otherwise, we a performing an explicit timeslice donation. Ensure that
     * the destination thread is valid. */
    tcb_t *dest_tcb;
    if (dest.is_threadhandle()) {
        dest_tcb = lookup_tcb_by_handle_locked(dest.get_raw());
    } else {
        dest_tcb = get_current_clist()->lookup_ipc_cap_locked(dest);
    }

    /* If we are donating to ourself, or invalid thread we have no work to do. */
    if (EXPECT_FALSE(dest_tcb == NULL || dest_tcb == current)) {
        if (dest_tcb != NULL) {
            dest_tcb->unlock_read();
        }
        return_thread_switch(continuation);
    }
    dest_tcb->unlock_read();

    /* Donate timeslice to the given thread. */
    scheduler->donate(dest_tcb, get_current_tcb(), continuation);
    NOTREACHED();
}


/* local part of schedule */
bool
scheduler_t::do_schedule(tcb_t * tcb, word_t ts_len, word_t prio, word_t flags)
{
    bool schedule_required = false;
#if 0

    /* Change priority of the thread if requested. */
    if ((prio != (~0UL)) && ((prio_t)prio != tcb->base_prio)) {
        set_priority(tcb, (prio_t)(prio & 0xff));
        schedule_required = true;
    }

    /* Modify the threads timeslice length if requested. */
    if (ts_len != (~0UL)) {
        set_timeslice_length(tcb, ts_len);
    }
#endif

    return schedule_required;
}

static CONTINUATION_FUNCTION(finish_sys_schedule);

SYS_SCHEDULE(capid_t dest_tid, word_t ts_len, word_t context_bitmask,
        word_t domain_control, word_t prio, word_t flags)
{
    LOCK_PRIVILEGED_SYSCALL();
    continuation_t continuation = ASM_CONTINUATION;
    tcb_t * current = get_current_tcb();
    tcb_t * dest_tcb;
    scheduler_t * scheduler = get_current_scheduler();
    bool schedule_required = false;

    space_t * space = get_current_space();

    if (dest_tid.is_myself()) {
        dest_tcb = acquire_read_lock_current(current);
    } else {
        /*
        * Only allow schedule if:
        *  - thread cap,
        *  or
        *  - ipc cap and we are in the same address space as target thread.
        */
        dest_tcb = get_current_clist()->lookup_thread_cap_locked(dest_tid);
        if (dest_tcb == NULL) {
            dest_tcb = get_current_clist()->lookup_ipc_cap_locked(dest_tid);
            if (EXPECT_FALSE( dest_tcb && (space != dest_tcb->get_space())))
            {
                current->set_error_code (EINVALID_SPACE);
                goto error_out_locked;
            }
        }
    }

    /* make sure the thread is valid */
    if (EXPECT_FALSE(dest_tcb == NULL))
    {
        current->set_error_code (EINVALID_THREAD);
        goto error_out;
    }

    /* Check that we are not setting the thread's priority higher than we are
     * allowed to. */
    if (prio != (~0UL) && ((prio_t)(prio & 0xff) > space->get_maximum_priority())) {
        current->set_error_code (EINVALID_PARAM);
        goto error_out_locked;
    }

    current->sys_data.set_action(tcb_syscall_data_t::action_schedule);

    TCB_SYSDATA_SCHED(current)->dest_tcb = dest_tcb;

    if (dest_tcb->is_local_domain()) {
        UNLOCK_PRIVILEGED_SYSCALL();
        scheduler->pause(dest_tcb);
        LOCK_PRIVILEGED_SYSCALL();
        dest_tcb->unlock_read();

        schedule_required = scheduler->do_schedule(dest_tcb,
                ts_len, prio, flags);

        if ((domain_control != ~0UL) &&
            (domain_control < (word_t)get_mp()->get_num_scheduler_domains())) {
        }
    }
    /* FIXME: this stuff needs to be updated */
    else {
        UNIMPLEMENTED();
    }

    /* Save the sys_schedule return address */
    TCB_SYSDATA_SCHED(current)->schedule_continuation = continuation;

    /* Unpause the thread. */
    if (get_current_tcb() != dest_tcb) {
        scheduler->unpause(dest_tcb);
    }

    /* Perform a schedule if we have modified the scheduling parameters of any
     * thread, which may affect which thread should now be running. */
    UNLOCK_PRIVILEGED_SYSCALL();
    if (schedule_required) {
        scheduler->schedule(current, finish_sys_schedule,
                            scheduler_t::sched_default);
    } else {
        ACTIVATE_CONTINUATION(finish_sys_schedule);
    }
    NOTREACHED();

error_out_locked:
    dest_tcb->unlock_read();
error_out:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_schedule(0, 0, continuation);

}

/**
 *  Map a thread's state into a schedule system call return value.
 *
 *  @param state The thread state being inspected.
 *
 *  @return The schedule system call return value mapping to the
 *    inspected state.
 */

/**
 * Calculate the appropriate return value for the schedule
 * syscall, and then return to user
 *
 * This is a continuation function but does not use any arguments
 * stored in the TCB
 */


static
CONTINUATION_FUNCTION(finish_sys_schedule)
{
    LOCK_PRIVILEGED_SYSCALL();
    tcb_t * current = get_current_tcb();
    tcb_t * dest_tcb = TCB_SYSDATA_SCHED(current)->dest_tcb;

    thread_state_t state = dest_tcb->get_state();

    UNLOCK_PRIVILEGED_SYSCALL();

    return_schedule(L4_SCHEDRESULT_RUNNING, dest_tcb->current_timeslice,
        TCB_SYSDATA_SCHED(current)->schedule_continuation);

}

/**
 * Hook for platform code to cause a rescheduler.
 * This is used in the case of interrupt delivery
 */

extern "C"
void kernel_scheduler_handle_timer_interrupt(int wakeup, word_t timer_int, continuation_t cont)
{
}

NORETURN void
scheduler_t::fast_schedule(tcb_t * current, fast_schedule_type_e type,
                           tcb_t * tcb, continuation_t continuation,
                           flags_t flags = sched_default)
{
    /**
     * @todo FIXME: Mothra issue #2070. For SMT, check that threads
     * are allowed to run on the cores they are being scheduled on -
     * davidg.
     */
    bool can_schedule_current = type == current_tcb_schedulable;

    /* Determine which thread should be scheduled. */
    run_e action = should_schedule_thread(current, tcb,
            can_schedule_current, flags);

    switch (action) {
    case run_new:
        /* Should schedule the new thread. */
        if (flags & preempting_thread) {
            mark_thread_as_preempted(current);
        }
        switch_from(current, continuation);
        schedule_lock.lock();
        set_active_thread(tcb, tcb);
        schedule_lock.unlock();
        switch_to(tcb, tcb);
        NOTREACHED();
        break;

    case run_current:
        /* We should keep running. */
        ASSERT(DEBUG, can_schedule_current);
        schedule_lock.lock();
        schedule_lock.unlock();
        ACTIVATE_CONTINUATION(continuation);
        NOTREACHED();
        break;

    case full_schedule:
    default:
        /* Not sure: do a full schedule. */
        schedule_lock.lock();
        schedule_lock.unlock();
        schedule(current, continuation, flags);
        NOTREACHED();
        break;
    }
}

/**********************************************************************
 *
 *                     Initialisation
 *
 **********************************************************************/

void SECTION(SEC_INIT)
scheduler_t::start(cpu_context_t context)
{
    initial_switch_to(get_idle_tcb());
}

void SECTION(SEC_INIT)
scheduler_t::init(bool bootcpu)
{
}

