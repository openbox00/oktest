/*
 * Description: Scheduler's priority queue.
 */

#ifndef __PRIOQUEUE_H__
#define __PRIOQUEUE_H__

#include <arch/schedule.h>

#define BITMAP_WORDS ((MAX_PRIO + BITS_WORD) / BITS_WORD)

class prio_queue_t
{
public:
    void init(void);

    void enqueue(tcb_t * tcb);

    void dequeue(tcb_t * tcb);

    void set_base_priority(tcb_t * tcb, prio_t prio);

    void set_effective_priority(tcb_t * tcb, prio_t prio);

    void set(prio_t prio, tcb_t * tcb);

    tcb_t * get(prio_t prio);

    /**  Remove a bit in the priority queue's bitmap tree. */
    void remove_sched_bitmap_bit(word_t prio);

    /**  Remove a bit in the priority queue's bitmap tree. */
    void remove_sched_bitmap_bit(word_t level1_index, word_t level2_index);

    /** Head of the queues for each priority. */
    tcb_t * prio_queue[MAX_PRIO + 1];

public:

    /**
     * A bitmap indicating which array elements of 'prio_bitmap' contain
     * set bits.
     */
    word_t index_bitmap;

    /**
     * A bitmap consisting of one bit per priority. Each bit is set if
     * there may be a thread of that priority ready to be scheduled.
     */
    word_t prio_bitmap[BITMAP_WORDS];

    friend void mkasmsym(void);
};

/*
 * Public priority queue implementations.
 */

INLINE void
prio_queue_t::set(prio_t prio, tcb_t * tcb)
{
    this->prio_queue[prio] = tcb;
}

INLINE tcb_t *
prio_queue_t::get(prio_t prio)
{
    return prio_queue[prio];
}

INLINE void
prio_queue_t::remove_sched_bitmap_bit(word_t level1_index, word_t level2_index)
{
    word_t level2_bitmap = prio_bitmap[level1_index];
    level2_bitmap &= ~(1UL << level2_index);
    prio_bitmap[level1_index] = level2_bitmap;
    if (level2_bitmap == 0) {
        index_bitmap &= ~(1UL << level1_index);
    }
}

INLINE void
prio_queue_t::remove_sched_bitmap_bit(word_t prio)
{
    remove_sched_bitmap_bit(prio / BITS_WORD, prio % BITS_WORD);
}

#endif /*__PRIOQUEUE_H__*/
