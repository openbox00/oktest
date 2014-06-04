#include <l4.h>
#include <tcb.h>
#include <queueing.h>
#include <sync.h>
#include <schedule.h>

void
syncpoint_t::init(tcb_t * donatee)
{
    scheduler_t * scheduler = get_current_scheduler();

    scheduler->scheduler_lock();
    this->donatee = donatee;
    this->blocked_head = NULL;
    scheduler->scheduler_unlock();
}

void
syncpoint_t::block(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));
    ASSERT(ALWAYS, !tcb->blocked_list.is_queued());
    ASSERT(ALWAYS, tcb->is_local_unit());

    this->enqueue_blocked(tcb);

#ifdef CONFIG_SCHEDULE_INHERITANCE
    /* If somebody is the donatee of this syncpoint, propagate
     * our priority to them. */
    if (this->donatee) {
        this->propagate_priority_increase(tcb->effective_prio);
    }
#endif

#if defined(CONFIG_TRACEPOINTS)
    int head_length, loop_length;
    if (is_deadlocked(tcb, &head_length, &loop_length)) {
        TRACEPOINT(DEADLOCK_DETECTED,
                report_deadlock(tcb, head_length, loop_length));
    }
#endif
}

void
syncpoint_t::unblock(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));
    ASSERT(ALWAYS, tcb->is_local_unit());
    ASSERT(ALWAYS, tcb->blocked_list.is_queued());

    this->dequeue_blocked(tcb);

#ifdef CONFIG_SCHEDULE_INHERITANCE
    /* If somebody was inheriting our priority, we need to propagate
     * the changes down the chain. */
    if (this->donatee) {
        this->propagate_priority_change();
    }
#endif
}

void
syncpoint_t::refresh(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /* Requeue the thread into its new position. */
    this->dequeue_blocked(tcb);
    this->enqueue_blocked(tcb);

#ifdef CONFIG_SCHEDULE_INHERITANCE
    /* If somebody was inheriting our priority, we need to propagate
     * the changes down the chain. */
    if (this->donatee) {
        this->propagate_priority_change();
    }
#endif
}

void
syncpoint_t::become_donatee(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /* Ensure we don't already have a donatee. */
    ASSERT(ALWAYS, donatee == NULL);

    /**
     *  @todo FIXME: The below comment is mutex specific and doesn't
     *  apply syncpoint's in general - awiggins.
     */
    
    /*
     * For the moment, threads only become the donatee when they are
     * the highest priority thread on the syncpoint, and hence no
     * priority inheritance needs to take place.
     *
     * This code will need to be updated if this assumption changes.
     *
     */

    ASSERT(ALWAYS, !blocked_head
            || blocked_head->effective_prio <= tcb->effective_prio);

    donatee = tcb;
}

void
syncpoint_t::release_donatee()
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /* Ensure we have a donatee. */
    ASSERT(ALWAYS, donatee != NULL);

    /* Can only release a donatee if they are not blocked on something else
     * first. Threads being deleted should be "tcb_t::unwind()"ed first. */
    ASSERT(ALWAYS, donatee->get_waiting_for() == NULL);

#ifdef CONFIG_SCHEDULE_INHERITANCE
    /* Ensure that the donatee's priority is sane. It should be at least
     * as high as the highest thread on the queue (because we should be
     * inheriting it). */
    ASSERT(ALWAYS, !blocked_head
            || blocked_head->effective_prio <= donatee->effective_prio);

    /* We also assume that the donatee is the thread responsible for doing the
     * release, and hence is off the scheduling queue. If this assumption
     * becomes false, this code will need to be redesinged to handle cases
     * where the thread is (i) on the scheduling queue; and (ii) has a priority
     * change. */
    ASSERT(ALWAYS, !donatee->ready_list.is_queued());

    /* Update the donatee's effective priority. */
    if (blocked_head && blocked_head->effective_prio
            >= donatee->effective_prio) {
        donatee->effective_prio = donatee->calc_effective_priority();
    }
#endif

    /* Finally, update the structure. */
    donatee = NULL;
}

/* Private methods */

void
syncpoint_t::enqueue_tcb_sorted(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /* Enqueue the TCB in the correct priority ordering. */
    prio_t p = tcb->effective_prio;

    /* If nobody else is in the list, just add it in. */
    if (this->blocked_head == NULL) {
        this->blocked_head = tcb;
        tcb->blocked_list.next = tcb;
        tcb->blocked_list.prev = tcb;
        return;
    }

    /* Otherwise, find it in the list. */
    tcb_t * head = this->blocked_head;
    tcb_t * first = this->blocked_head;

    while (p <= head->effective_prio) {
        head = head->blocked_list.next;
        if (head == first) {
            break;
        }
    }

    /* And add it in its correct position. */
    tcb->blocked_list.next = head;
    tcb->blocked_list.prev = head->blocked_list.prev;
    head->blocked_list.prev->blocked_list.next = tcb;
    head->blocked_list.prev = tcb;

    /* If it is the first item in the list, update blocked head. */
    if (p > this->blocked_head->effective_prio) {
        this->blocked_head = tcb;
    }
}

#if !(defined(HAVE_ENQUEUE_BLOCKED_FASTPATH) && defined(CONFIG_ENABLE_FASTPATHS))
void
syncpoint_t::enqueue_blocked(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));
    ASSERT(ALWAYS, tcb);
    ASSERT(ALWAYS, tcb->get_waiting_for() == NULL);
    ASSERT(ALWAYS, tcb->blocked_list.next == NULL);

    this->enqueue_tcb_sorted(tcb);
    tcb->set_waiting_for(this);
}
#endif

#if !defined(CONFIG_ENABLE_FASTPATHS) \
        || !defined(HAVE_SYNCPOINT_DEQUEUE_BLOCKED_FASTPATH)
void
syncpoint_t::dequeue_blocked(tcb_t * tcb)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));
    ASSERT(ALWAYS, tcb);
    ASSERT(ALWAYS, tcb->get_waiting_for() == this);
    ASSERT(ALWAYS, tcb->blocked_list.next != NULL);

    DEQUEUE_LIST(tcb_t, this->blocked_head, tcb, blocked_list);
    tcb->set_waiting_for(NULL);
}
#endif

#if defined(CONFIG_SCHEDULE_INHERITANCE)
void
syncpoint_t::propagate_priority_increase(prio_t new_prio)
{
    tcb_t * chain = this->donatee;
    scheduler_t * scheduler = get_current_scheduler();

    SMT_ASSERT(ALWAYS, scheduler->schedule_lock.is_locked(true));

    while (chain) {
        /* Do we need to propagate priorities any further? */
        if (chain->effective_prio >= new_prio) {
            break;
        }

        /* Otherwise update the effective priority. */
        scheduler->set_effective_priority(chain, new_prio);

        /* Is this link in the chain waiting on someone else? */
        syncpoint_t * next = chain->get_waiting_for();

        if (next == NULL) {
            break;
        }

        /* Reposition this guy in its chain, and continue the walk. */
        next->dequeue_blocked(chain);
        next->enqueue_blocked(chain);
        chain = next->donatee;
    }
}

void
syncpoint_t::propagate_priority_change(void)
{
    tcb_t * chain = this->donatee;
    scheduler_t * scheduler = get_current_scheduler();

    SMT_ASSERT(ALWAYS, scheduler->schedule_lock.is_locked(true));

    while (chain) {
        /* Calculate this link in the chain's new priority. */
        prio_t old_prio = chain->effective_prio;
        prio_t new_prio = chain->calc_effective_priority();

        /* If there was no change in priority, we need not continue
         * to travel down the chain. */
        if (new_prio == old_prio) {
            break;
        }

        /* Otherwise update the effective priority. */
        scheduler->set_effective_priority(chain, new_prio);

        /* Is this link in the chain waiting on someone else? */
        syncpoint_t * next = chain->get_waiting_for();

        if (next == NULL) {
            break;
        }

        /* Reposition this guy in its chain, and continue the walk. */
        next->dequeue_blocked(chain);
        next->enqueue_blocked(chain);
        chain = next->donatee;
    }
}
#endif /* CONFIG_SCHEDULE_INHERITANCE */

#if defined(CONFIG_TRACEPOINTS)
/**
 * Find the next link in a syncpoint chain.
 */
static tcb_t *
next_link(tcb_t *current)
{
    syncpoint_t *next = current->get_waiting_for();
    if (next == NULL) {
        return NULL;
    }
    return next->get_donatee();
}

void syncpoint_t::report_deadlock(tcb_t *first,
        int head_length, int loop_length)
{
    /* Display the head, leading up to the loop. */
    for (int i = 0; i < head_length; i++) {
        printf("%t -> ", first);
        first = first->get_waiting_for()->donatee;
        ASSERT(ALWAYS, first != NULL);
    }

    /* Display the loop of the deadlock. */
    printf("[cycle start] ");
    for (int i = 0; i < loop_length; i++) {
        printf("%t -> ", first);
        first = first->get_waiting_for()->donatee;
        ASSERT(ALWAYS, first != NULL);
    }
    printf("%t [repeats]\n", first);
}

bool syncpoint_t::is_deadlocked(tcb_t *tcb, int *head_length, int *loop_length)
{
    SMT_ASSERT(ALWAYS, get_current_scheduler()->schedule_lock.is_locked(true));

    /*
     * We determine deadlock by using "Brent's Cycle Detection
     * Algorithm", which is fast in the non-deadlock case and (though we
     * don't actually care quite so much), quite fast in the deadlock
     * case too.
     *
     * Consider a loop in the linked list consisting of 'mu' links up
     * until the loop, followed by a loop of length 'lamda':
     *
     *      ----- MU = 3 -----   --- LAMDA = 3 ---
     *
     *     O ---> O ---> O ---> O ---> O ---> O -.
     *                          ^                |
     *                          |                |
     *                          `----------------'
     *
     * We search using two pointers. The first moves along the chain one
     * step at a time. Every so often, we move the second pointer to the
     * location of the first, and let the first keep going. This 'every
     * so often' happens at increasing powers of two: 1, 2, 4, 8, etc.
     *
     * Eventually, the first pointer will hit the end of the chain (and
     * we know we are deadlock free), or it will hit the second pointer.
     * If it hits the second pointer, we know that (i) there is a loop;
     * (ii) the second pointer is inside the loop; and (iii) the length
     * of the loop is equal to the number of steps the first pointer has
     * moved since we last changed the position of the second pointer.
     *
     * If we find a loop of known length, we can finally calculate the
     * length of 'mu' by having two pointers length 'lamda' away from
     * each other both moving along the chain. After 'mu' steps, the two
     * pointers will collide and we will have finished.
     *
     * So here we go...
     */
    tcb_t *fast_chain = next_link(tcb);
    tcb_t *slow_chain = tcb;
    int lamda = 1;
    int power = 1;
    while (fast_chain != slow_chain) {
        /* If we have reached the end of the chain, we do not have
         * deadlock. Let the caller know. */
        if (fast_chain == NULL) {
            return false;
        }

        /* Otherwise, see if we have reached the next power of two,
         * and hence need to move our second pointer. */
        if (lamda == power) {
            slow_chain = fast_chain;
            power *= 2;
            lamda = 0;
        }

        /* Move the first pointer forwards. */
        fast_chain = next_link(fast_chain);
        lamda++;
    }

    /* If we reach this point, we have a loop of length 'lamda'.
     * Calcuate the length of chain up to the loop. */
    fast_chain = tcb;
    slow_chain = tcb;

    /* Start by separating the two pointers by length 'lamda'. */
    int mu = 0;
    for (int i = 0; i < lamda; i++) {
        fast_chain = next_link(fast_chain);
        ASSERT(ALWAYS, fast_chain != NULL);
    }

    /* Now shuffle the pointers around the loop until they collide. */
    while (fast_chain != slow_chain) {
        fast_chain = next_link(fast_chain);
        slow_chain = next_link(slow_chain);
        ASSERT(ALWAYS, fast_chain != NULL);
        ASSERT(ALWAYS, slow_chain != NULL);
        mu++;
    }

    *loop_length = lamda;
    *head_length = mu;
    return true;
}
#endif /* CONFIG_TRACEPOINTS */

