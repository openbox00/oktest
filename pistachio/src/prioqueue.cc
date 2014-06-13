/*
 * Description: Scheduler priority queue functions.
 */
#include <l4.h>
//#include <debug.h>
#include <tcb.h>
#include <schedule.h>
#include <prioqueue.h>
#include <queueing.h>
#include <config.h>

void
prio_queue_t::enqueue(tcb_t * tcb)
{
    prio_t prio = tcb->effective_prio;

    /* Enqueue the TCB. */
    ENQUEUE_LIST_TAIL(tcb_t, prio_queue[prio], tcb, ready_list);

    /* Finally, update the bitmap tree. */
    index_bitmap |= (1UL << ((word_t)prio / BITS_WORD));
    prio_bitmap[(word_t)prio / BITS_WORD] |= 1UL << ((word_t)prio % BITS_WORD);
}

/* Dequeue the given TCB from the priority queue. */
void
prio_queue_t::dequeue(tcb_t * tcb)
{
    prio_t priority = tcb->effective_prio;

    DEQUEUE_LIST(tcb_t, prio_queue[priority], tcb, ready_list);
}

void
prio_queue_t::set_base_priority(tcb_t *tcb, prio_t new_priority)
{
    /* Recalculate effective priorities. */
    prio_t old_effective_prio = tcb->effective_prio;
    tcb->base_prio = new_priority;
    prio_t effective_prio = new_priority;
    /* If there is a change, we need to propagate it. */
    if (effective_prio != old_effective_prio) {
        set_effective_priority(tcb, effective_prio);
    }
}

void
prio_queue_t::set_effective_priority(tcb_t * tcb, prio_t new_priority)
{
    if (!tcb->ready_list.is_queued()) {
        tcb->effective_prio = new_priority;
        return;
    }
    dequeue(tcb);
    tcb->effective_prio = new_priority;
    enqueue(tcb);
}

