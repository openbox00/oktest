#ifndef __SYNCPOINT_H__
#define __SYNCPOINT_H__

#include <tcb.h>
#include <queueing.h>
#include <sync.h>

class tcb_t;

/** @todo FIXME: Doxygen comment for class - awiggins. */
class syncpoint_t
{
public:
    void init(tcb_t * donatee);

    /** Does this sync-point have threads blocked on it? */
    bool has_blocked(void);

    tcb_t * get_donatee(void);

    tcb_t * get_blocked_head(void);

    void block(tcb_t * tcb);

    NORETURN void block_sched(tcb_t * tcb, continuation_t continuation);

    void unblock(tcb_t * tcb);

    void refresh(tcb_t * tcb);

    void become_donatee(tcb_t * tcb);

    void release_donatee(void);

private:
    void enqueue_tcb_sorted(tcb_t * tcb);

    void enqueue_blocked(tcb_t * tcb);

    void dequeue_blocked(tcb_t * tcb);

    void propagate_priority_increase(prio_t new_prio);

    void propagate_priority_change(void);

    tcb_t * donatee;

    /** First TCB waiting on this syncpoint. */
    tcb_t * blocked_head;

    friend void mkasmsym(void);
};

INLINE bool
syncpoint_t::has_blocked(void)
{
    return this->get_blocked_head() != NULL;
}

INLINE tcb_t *
syncpoint_t::get_donatee(void)
{
    return this->donatee;
}


INLINE tcb_t *
syncpoint_t::get_blocked_head(void)
{
    return this->blocked_head;
}


#endif /* !__SYNCPOINT_H__ */
