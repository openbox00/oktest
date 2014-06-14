#include <tcb.h>
#include <prioqueue.h>

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


