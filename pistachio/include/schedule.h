/*
 * Description:   scheduling declarations
 */

#ifndef __SCHEDULE_H__
#define __SCHEDULE_H__

#include <types.h>
#include <tcb.h>
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
private:
    void enqueue(tcb_t * tcb);
    void dequeue(tcb_t * tcb);
public:
    void set_priority(tcb_t * tcb, prio_t prio);
    void set_effective_priority(tcb_t *tcb, prio_t prio);

public:
    void init_tcb(tcb_t * tcb, prio_t prio, word_t timeslice_length,
            word_t context_bitmask);
private:
    word_t get_context_bitmask(tcb_t * tcb);
    void end_of_timeslice(tcb_t * tcb);
    void set_timeslice_length(tcb_t * tcb, word_t timeslice);
    prio_t get_highest_priority();
    void remove_sched_bitmap_bit(word_t level1_index, word_t level2_index);
    prio_queue_t prio_queue;
    static volatile u64_t current_time;

public:
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

/**********************************************************************
 *
 *                   Public Scheduler Methods
 *
 **********************************************************************/
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
    set_priority(tcb, prio);
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
