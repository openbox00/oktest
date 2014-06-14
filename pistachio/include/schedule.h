#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include <tcb.h>
#include <prioqueue.h>

class scheduler_t
{
public:
    NORETURN void schedule(tcb_t * current, continuation_t continuation);
private:
    void enqueue(tcb_t * tcb);
public:
private:
    prio_t get_highest_priority();
    prio_queue_t prio_queue;
public:
    friend void set_running_and_enqueue(tcb_t * tcb);
};

INLINE void
scheduler_t::enqueue(tcb_t * tcb)
{
    prio_queue.enqueue(tcb);
}


CONST INLINE scheduler_t *
get_current_scheduler(void)
{
    extern scheduler_t __scheduler;
    return &__scheduler;
}

#endif /* !__SCHEDULE_H__ */
