/*
 * Description:   scheduling declarations
 */

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include <types.h>
#include <tcb.h>
#include <arch/schedule.h>
#include <prioqueue.h>

class scheduler_t
{
public:
    typedef word_t flags_t;
    static const flags_t sched_default = 0;
    static const flags_t preempting_thread = 1 << 0;
    static const flags_t sched_round_robin = 1 << 1;
    void init(bool bootcpu = true);
    NORETURN void start(cpu_context_t context);
    NORETURN void schedule(tcb_t * current, continuation_t continuation,
                           flags_t flags);
    NORETURN void donate(tcb_t * dest_tcb, tcb_t * current,
                continuation_t continuation);
    void activate(tcb_t * tcb, thread_state_t new_state,
            flags_t flags = sched_default);
    void deactivate(tcb_t * tcb, thread_state_t new_state,
            flags_t flags = sched_default);
    NORETURN void activate_sched(tcb_t * tcb, thread_state_t new_state,
            tcb_t * current, continuation_t continuation, flags_t flags);
    NORETURN void deactivate_sched(tcb_t * tcb, thread_state_t new_state,
            tcb_t * current, continuation_t continuation, flags_t flags);
    NORETURN void deactivate_activate_sched(tcb_t * deactivate_tcb,
            tcb_t * activate_tcb, thread_state_t new_inactive_state,
            thread_state_t new_active_state, tcb_t * current,
            continuation_t continuation, flags_t flags);
    void update_active_state(tcb_t * tcb, thread_state_t new_state);
    void update_inactive_state(tcb_t * tcb, thread_state_t new_state);
    NORETURN void context_switch(tcb_t * current, tcb_t * dest,
            thread_state_t new_current_state, thread_state_t new_dest_state,
            continuation_t cont);
private:
    void enqueue(tcb_t * tcb);
    void dequeue(tcb_t * tcb);
public:
    void set_priority(tcb_t * tcb, prio_t prio);
    void set_effective_priority(tcb_t *tcb, prio_t prio);

public:
    void init_tcb(tcb_t * tcb, prio_t prio, word_t timeslice_length,
            word_t context_bitmask);
    void handle_timer_interrupt(bool wakeup, word_t timer_length, continuation_t);

public:
    bool do_schedule(tcb_t * tcb, word_t ts_len, word_t prio, word_t flags);
    void set_active_thread(tcb_t * tcb, tcb_t * schedule);
    void delete_tcb(tcb_t * tcb);
    NORETURN void yield(tcb_t * current, tcb_t * active_schedule,
            continuation_t continuation);

private:
    void smt_reschedule(tcb_t * tcb);
    void release_and_reschedule_tcb(tcb_t * tcb);
    void mark_thread_as_preempted(tcb_t * tcb);
    typedef enum {
        current_tcb_schedulable,
        current_tcb_unschedulable,
    } fast_schedule_type_e;

    NORETURN void fast_schedule(tcb_t * current, fast_schedule_type_e type,
            continuation_t continuation, flags_t flags);

    NORETURN void fast_schedule(tcb_t * current,
            fast_schedule_type_e type, tcb_t * tcb,
            continuation_t continuation, flags_t flags);

    typedef enum {
        run_current,
        run_new,
        full_schedule
    } run_e;

    run_e should_schedule_thread(tcb_t * current, tcb_t * tcb, bool can_schedule_current, flags_t flags);
    word_t get_context_bitmask(tcb_t * tcb);
    void end_of_timeslice(tcb_t * tcb);
    void set_timeslice_length(tcb_t * tcb, word_t timeslice);
    prio_t get_highest_priority();
    void remove_sched_bitmap_bit(word_t level1_index, word_t level2_index);
    prio_queue_t prio_queue;
    static volatile u64_t current_time;

public:
    spinlock_t schedule_lock;
    bool scheduling_invariants_violated;
    friend void mkasmsym(void);
    friend void switch_from(tcb_t * current, continuation_t continuation);
    friend void switch_to(tcb_t * dest, tcb_t * schedule);
    friend void set_running_and_enqueue(tcb_t * tcb);
};

/**********************************************************************
 *
 *             Scheduler configuration helper methods
 *
 **********************************************************************/

INLINE void
scheduler_t::enqueue(tcb_t * tcb)
{
    if (tcb->is_reserved()) {
        return;
    }
    prio_queue.enqueue(tcb);
}

INLINE void
scheduler_t::dequeue(tcb_t * tcb)
{
    prio_queue.dequeue(tcb);
}

INLINE NORETURN void
scheduler_t::fast_schedule(tcb_t * current, fast_schedule_type_e type,
                           continuation_t continuation,
                           flags_t flags = sched_default)
{
    bool can_schedule_current = (type == current_tcb_schedulable);
    if (EXPECT_TRUE(can_schedule_current && !scheduling_invariants_violated)) {
        ACTIVATE_CONTINUATION(continuation);
    }
    schedule(current, continuation, flags);
}

INLINE scheduler_t::run_e
scheduler_t::should_schedule_thread(tcb_t * current, tcb_t * tcb,
        bool can_schedule_current, flags_t flags)
{
    if (EXPECT_FALSE(scheduling_invariants_violated)) {
        return full_schedule;
    }
    if (EXPECT_TRUE(can_schedule_current
                && current->effective_prio >= tcb->effective_prio)) {
        return run_current;
    }
    if (tcb->effective_prio >= current->effective_prio) {
        return run_new;
    }
    return full_schedule;
}

INLINE word_t
scheduler_t::get_context_bitmask(tcb_t * tcb)
{
    return (word_t)-1;
}

INLINE void
scheduler_t::set_timeslice_length(tcb_t * tcb, word_t timeslice)
{
    tcb->current_timeslice = tcb->timeslice_length = timeslice;
}

/**********************************************************************
 *
 *                   Public Scheduler Methods
 *
 **********************************************************************/

INLINE void
scheduler_t::activate(tcb_t * tcb, thread_state_t new_state, flags_t flags)
{
    tcb->set_state(new_state);
}

INLINE void
scheduler_t::deactivate(tcb_t * tcb, thread_state_t new_state, flags_t flags)
{
    tcb->set_state(new_state);
    if (tcb->ready_list.is_queued()) {
        dequeue(tcb);
    }
}

INLINE NORETURN void
scheduler_t::donate(tcb_t * dest_tcb, tcb_t * current,
                    continuation_t continuation)
{
    if (!dest_tcb->get_state().is_runnable()
            || !dest_tcb->ready_list.is_queued() || !dest_tcb->grab()) {
        schedule_lock.unlock();
        yield(current, get_active_schedule(), continuation);
        NOTREACHED();
    }
    dequeue(dest_tcb);
    switch_from(current, continuation);
    current->release();
    enqueue(current);
    switch_to(dest_tcb, get_active_schedule());
}

INLINE void
scheduler_t::activate_sched(tcb_t * tcb, thread_state_t new_state,
                            tcb_t * current, continuation_t continuation,
                            flags_t flags)
{
    tcb->set_state(new_state);
}

INLINE void
scheduler_t::deactivate_sched(tcb_t * tcb, thread_state_t new_state,
                              tcb_t * current, continuation_t continuation,
                              flags_t flags)
{
    tcb->set_state(new_state);
    if (tcb != current) {
        if (tcb->ready_list.next != NULL) {
            schedule_lock.lock();
            dequeue(tcb);
            schedule_lock.unlock();
        }
        tcb->release();
    }
    fast_schedule(current, current != tcb ?
                  current_tcb_schedulable : current_tcb_unschedulable,
                  continuation, flags);
}

INLINE void
scheduler_t::deactivate_activate_sched(tcb_t * deactivated_tcb,
                                       tcb_t * activated_tcb,
                                       thread_state_t new_inactive_state,
                                       thread_state_t new_active_state,
                                       tcb_t * current,
                                       continuation_t continuation,
                                       flags_t flags)
{
    if (deactivated_tcb != current) {
        schedule_lock.lock();
        dequeue(deactivated_tcb);
        schedule_lock.unlock();
    }

    deactivated_tcb->set_state(new_inactive_state);
    activated_tcb->set_state(new_active_state);

    if (deactivated_tcb != current) {
        deactivated_tcb->release();
    }

    fast_schedule(current,
            current != deactivated_tcb
            ? current_tcb_schedulable : current_tcb_unschedulable,
            activated_tcb, continuation, flags);
}

INLINE void
scheduler_t::update_active_state(tcb_t * tcb, thread_state_t new_state)
{
    tcb->set_state(new_state);
}

INLINE void
scheduler_t::update_inactive_state(tcb_t * tcb, thread_state_t new_state)
{
    tcb->set_state(new_state);
}

INLINE void
scheduler_t::set_priority(tcb_t * tcb, prio_t prio)
{
    prio_queue.set_base_priority(tcb, prio);
}

INLINE void
scheduler_t::init_tcb(tcb_t * tcb, prio_t prio = DEFAULT_PRIORITY,
                      word_t timeslice_length = DEFAULT_TIMESLICE_LENGTH,
                      word_t context_bitmask = DEFAULT_CONTEXT_BITMASK)
{
    set_timeslice_length(tcb, timeslice_length);
    set_priority(tcb, prio);
}

INLINE void
scheduler_t::delete_tcb(tcb_t * tcb)
{
    tcb->set_state(thread_state_t::aborted);
    tcb->base_prio = 0;
    tcb->effective_prio = 0;
}

INLINE void
scheduler_t::mark_thread_as_preempted(tcb_t * tcb)
{
        tcb->set_preempted_ip(tcb->get_user_ip());
        tcb->set_user_ip(tcb->get_preempt_callback_ip());
}


INLINE NORETURN void
scheduler_t::context_switch(tcb_t * current, tcb_t * dest,
        thread_state_t new_current_state, thread_state_t new_dest_state,
        continuation_t cont)
{
    current->set_state(new_current_state);
    dest->set_state(new_dest_state);
    schedule_lock.lock();
    scheduling_invariants_violated = true;
    switch_from(current, cont);
    if (current->get_state().is_runnable()) {
        enqueue(current);
    }
    schedule_lock.unlock();
    switch_to(dest, get_active_schedule());
}
/**********************************************************************
 *
 *                  Global function declarations
 *
 **********************************************************************/
CONST INLINE scheduler_t *
get_current_scheduler(void)
{
    extern scheduler_t __scheduler;

    return &__scheduler;
}

#endif /* !__SCHEDULE_H__ */
