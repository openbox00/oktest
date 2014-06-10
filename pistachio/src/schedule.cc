/*
 * Description:   Scheduling functions
 */
#include <tcb.h>
#include <schedule.h>
#include <interrupt.h>
#include <queueing.h>

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
    prio_queue.set_effective_priority(tcb, prio);
}

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

    /* Switch away from the current thread, enqueuing if necessary. */
    switch_from(current, continuation);
    current->release();

	if (current_runnable && current != get_idle_tcb()) {
        enqueue(current);
    }

    /* Activate the new thread. */
    schedule_lock.unlock();
    switch_to(next, next);
}

INLINE NORETURN void
scheduler_t::handle_timer_interrupt(bool wakeup, word_t timer_length, continuation_t continuation)
{
}

extern "C"
void kernel_scheduler_handle_timer_interrupt(int wakeup, word_t timer_int, continuation_t cont)
{
}


