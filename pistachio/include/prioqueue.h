#ifndef __PRIOQUEUE_H__
#define __PRIOQUEUE_H__

#define BITMAP_WORDS ((MAX_PRIO + BITS_WORD) / BITS_WORD)

class prio_queue_t
{
public:
    void enqueue(tcb_t * tcb);
    void dequeue(tcb_t * tcb);
    void set_base_priority(tcb_t * tcb, prio_t prio);
    void set_effective_priority(tcb_t * tcb, prio_t prio);
    tcb_t * get(prio_t prio);
    tcb_t * prio_queue[MAX_PRIO + 1];

public:
    word_t index_bitmap;
    word_t prio_bitmap[BITMAP_WORDS];
};

INLINE tcb_t *
prio_queue_t::get(prio_t prio)
{
    return prio_queue[prio];
}



#endif /*__PRIOQUEUE_H__*/
