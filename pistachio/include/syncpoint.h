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

    /**
     *  Initialise this syncpoint.
     *
     *  @param donatee The TCB reference of the initial donatee thread
     *  for this syncpoint.
     */
    void init(tcb_t * donatee);

    /** Does this sync-point have threads blocked on it? */
    bool has_blocked(void);

    /**
     * Inspect this synchronisation point's schedule donatee.
     *
     * @return The TCB of the thread receiving schedules inherited via
     * this synchronisation point.
     */
    tcb_t * get_donatee(void);

    /**
     * Return the head of this synchronisation points blocked queue.
     *
     * @return this->blocked_head
     */
    tcb_t * get_blocked_head(void);

    /**
     * Cause a thread to become blocked waiting on the donatee of this
     * syncpoint.
     *
     * @param tcb Reference to the thread to add to this sync-point's
     *   blocking queue.
     */
    void block(tcb_t * tcb);

    /**
     * Cause a thread to become blocked waiting on the donatee of this
     * syncpoint, and perform a schedule. By performing both operations as a
     * single call, optimisations may be made speeding up scheduling.
     *
     * @param tcb Reference to the thread to add to this sync-point's
     *   blocking queue.
     * @parma continuation ...
     */
    NORETURN void block_sched(tcb_t * tcb, continuation_t continuation);

    /**
     * Unblock a thread on a syncpoint.
     *
     * This operation is potentially slow if the syncpoint the thread is
     * being unblocked from has a donatee.
     *
     * @param tcb ...
     */
    void unblock(tcb_t * tcb);

    /**
     * Update this syncpoint to take into account priority changes
     * by the given thread.
     *
     * @param tcb ...
     */
    void refresh(tcb_t * tcb);

    /**
     * Become the donatee of this syncpoint.
     *
     * @param tcb ...
     */
    void become_donatee(tcb_t * tcb);

    /**
     * Cause the current syncpoint to clear its current donatee.
     */
    void release_donatee(void);

private:

    /**
     * Enqueue the given TCB in effective priority ordering into this
     * syncpoints blocked list.
     *
     * @param tcb ...
     *
     */
    void enqueue_tcb_sorted(tcb_t * tcb);

    /**
     * Add a thread to the queue of threads blocked on this
     * synchronisation point.
     *
     * @pre        'tcb' held by the currently running thread.
     *
     * @param tcb  TCB of thread blocking on this synchronisation
     * point.
     */
    void enqueue_blocked(tcb_t * tcb);

    /**
     * Remove a thread from the queue of threads blocked on this
     * synchronisation point.
     *
     * @pre        'tcb' held by the currently running thread.
     *
     * @param tcb TCB of thread unblocking from this synchronisation
     * point.
     */
    void dequeue_blocked(tcb_t * tcb);

    /**
     * Update each thread's priority down the dependency chain until
     * we hit the end of the chain, or hit a thread whose priority
     * does not increase.
     *
     * This is faster than the more generic
     * 'propagate_priority_change' because we don't have to
     * recalculate each thread's priority: merely increase it if it is
     * not high enough already.
     *
     * Deadlocks (i.e., loops in the dependency chain) are magically
     * handled by the fact we eventually reach a thread which already
     * has the correct priority, and hence break out of the loop.
     *
     * @param new_prio ...
     */
    void propagate_priority_increase(prio_t new_prio);

    /**
     * Update each thread's priority down the dependency chain until
     * we hit the end of the chain, or somebody's priority remains
     * unchanged.
     *
     * This function is slower than 'propagate_priority_increase' as
     * we must do more work on each thread to determine its modified
     * priority.
     *
     * Deadlocks (i.e., loops in the dependency chain) are magically
     * handled by the fact we eventually reach a thread which already
     * has the correct priority, and hence break out of the loop.
     */
    void propagate_priority_change(void);

#if defined(CONFIG_TRACEPOINTS)
    /**
     * Determine if the given thread is in a deadlocked state.
     *
     * @param tcb          The TCB to start the search on.
     *
     * @param head_length  If a deadlock is found, the number of TCBs
     *                     that are in the chain until the first TCB
     *                     in the deadlock loop is found.
     *
     * @param loop_length  If a deadlock is found, the number of threads in
     *                     the deadlock loop.
     *
     * @returns            True if a deadlock was found, false otherwise.
     */
    static bool is_deadlocked(tcb_t *tcb, int *head_length, int *loop_length);

    /**
     * Report to the user the presense of deadlock.
     *
     * @param tcb          The TCB to start reporting on.
     *
     * @param head_length  The number of TCBs in the chain until the
     *                     deadlock loop starts.
     *
     * @param loop_length  The number of TCBS in the deadlock loop.
     */
    static void report_deadlock(tcb_t *first, int head_length,
            int loop_length);
#endif /* CONFIG_TRACEPOINTS */

    /**
     * The current holder of this syncpoint, and who schedules will be
     * forwarded to if a thread is currently blocked on us.
     */
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
