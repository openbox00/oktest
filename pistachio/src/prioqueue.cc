/*
 * Description: Scheduler priority queue functions.
 */

#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <schedule.h>
#include <prioqueue.h>
#include <queueing.h>
#include <smp.h>
#include <config.h>

/* Initialise the priority queue data structure. */
void
prio_queue_t::init(void)
{
#if defined(CONFIG_DEBUG)
    word_t i;
    /* prio queues - 0 to MAX_PRIO inclusive! */
    for (i = 0; i < (MAX_PRIO + 1); i++) {
        ASSERT(ALWAYS, prio_queue[i] == NULL);
    }
    ASSERT(ALWAYS, index_bitmap == 0);
    for (i = 0; i < BITMAP_WORDS; i++) {
        ASSERT(ALWAYS, prio_bitmap[i] == 0);
    }
#endif
}

void
prio_queue_t::enqueue(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));
    SMT_ASSERT(ALWAYS, !tcb->is_grabbed_by_me());
    ASSERT(ALWAYS, tcb != get_idle_tcb());
    ASSERT(ALWAYS, !tcb->ready_list.is_queued());

    prio_t prio = tcb->effective_prio;

    /* Enqueue the TCB. */
    ASSERT(DEBUG, prio >= 0 && prio <= MAX_PRIO);
    ENQUEUE_LIST_TAIL(tcb_t, prio_queue[prio], tcb, ready_list);

    /* Finally, update the bitmap tree. */
    index_bitmap |= (1UL << ((word_t)prio / BITS_WORD));
    prio_bitmap[(word_t)prio / BITS_WORD] |= 1UL << ((word_t)prio % BITS_WORD);
}

/* Dequeue the given TCB from the priority queue. */
void
prio_queue_t::dequeue(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));
    ASSERT(DEBUG, tcb);
    ASSERT(ALWAYS, tcb != get_idle_tcb());
    ASSERT(ALWAYS, tcb->ready_list.is_queued());

    prio_t priority = tcb->effective_prio;

    ASSERT(DEBUG, priority >= 0 && priority <= MAX_PRIO);
    DEQUEUE_LIST(tcb_t, prio_queue[priority], tcb, ready_list);
    if (prio_queue[priority] == 0) {
        remove_sched_bitmap_bit(priority);
    }
}

void
prio_queue_t::set_base_priority(tcb_t *tcb, prio_t new_priority)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /* Recalculate effective priorities. */
    prio_t old_effective_prio = tcb->effective_prio;
    tcb->base_prio = new_priority;

#ifdef CONFIG_SCHEDULE_INHERITANCE
    prio_t effective_prio = tcb->calc_effective_priority();
#else
    prio_t effective_prio = new_priority;
#endif

    /* If there is a change, we need to propagate it. */
    if (effective_prio != old_effective_prio) {
        set_effective_priority(tcb, effective_prio);
    }
}

void
prio_queue_t::set_effective_priority(tcb_t * tcb, prio_t new_priority)
{
    ASSERT(DEBUG, new_priority >= 0 && new_priority <= MAX_PRIO);
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /* Thread is not on the priority queue. Just update its priority,
     * with no more to be done. */
    if (!tcb->ready_list.is_queued()) {
        tcb->effective_prio = new_priority;
        return;
    }

    /* Otherwise, dequeue the thread and enqueue it again at its new
     * priority. */
    dequeue(tcb);
    tcb->effective_prio = new_priority;
    enqueue(tcb);
}

