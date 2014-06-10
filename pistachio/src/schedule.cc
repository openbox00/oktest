/*
 * Description:   Scheduling functions
 */
#include <tcb.h>
#include <schedule.h>
#include <queueing.h>

prio_t
scheduler_t::get_highest_priority(void)
{
    word_t first_level_bitmap = prio_queue.index_bitmap;
    if (!first_level_bitmap) {
        return (prio_t)-1;
    }
    word_t first_level_index = msb(first_level_bitmap);
    word_t second_level_bitmap = prio_queue.prio_bitmap[first_level_index];
    word_t second_level_index = msb(second_level_bitmap);
    prio_t top_prio = first_level_index * BITS_WORD + second_level_index;
    return top_prio;
}
void
scheduler_t::schedule(tcb_t * current, continuation_t continuation,
                      flags_t flags)
{
    //scheduling_invariants_violated = false;
    prio_t max_prio = get_highest_priority();
    bool current_runnable = current->get_state().is_runnable() && !current->is_reserved();
    if (current_runnable) {
        if (current->effective_prio > max_prio ||
                (!(flags & sched_round_robin)
                        && current->effective_prio == max_prio)) {
            schedule_lock.unlock();
            ACTIVATE_CONTINUATION(continuation);
        }
        if (flags & preempting_thread) {
            mark_thread_as_preempted(current);
        }
    }
    tcb_t *next;
    if (max_prio >= 0) {
        next = prio_queue.get(max_prio);
        dequeue(next);
    } else {
        next = get_idle_tcb();
    }
    (void)next->grab();
    switch_from(current, continuation);
    current->release();

	if (current_runnable && current != get_idle_tcb()) {
        enqueue(current);
    }
    switch_to(next, next);
}

