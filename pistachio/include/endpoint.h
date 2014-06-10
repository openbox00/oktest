#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

class tcb_t;

/**
 *  @brief End-point objects. End-points are a form of synchronisation
 *  point which implement inter-process communication for users.
 *
 *  The end-point's send and receive queues are both backed by the
 *  sync_point member's blocked-queue. As end-points are currently
 *  tied a single thread the receive-queue is only used by schedule
 *  inheritance and is conditionally compiled in.
 */
class endpoint_t
{
    friend class kdb_t;
public:

    /**
     *  Initialise this syncpoint.
     *
     *  @param tcb The TCB reference of thread this end-point is a
     *  part of.
     */
    void init(tcb_t * tcb);

    /**
     *  Find the head of this end-point's send-queue, if any.
     *
     *  @return The TCB reference of the head thread.
     *  @retval NULL This end-point's send-queue is empty.
     */
    tcb_t * get_send_head(void);

    /**
     *  Enqueue argument thread onto this end-point's send-queue.
     *
     *  @param tcb The TCB reference of the thread to enqueue.
     */
    void enqueue_send(tcb_t * tcb);

    /**
     *  Dequeue argument thread from this end-point's send-queue.
     *
     *  @param tcb The TCB reference of the thread to dequeue.
     */
    void dequeue_send(tcb_t * tcb);

    /**
     *  Find the head of this end-point's receive-queue, if any.
     *
     *  @return The TCB reference of the head thread.
     *  @retval NULL This end-point's receive-queue is empty.
     */
    tcb_t * get_recv_head(void);

    /**
     *  Enqueue argument thread onto this end-point's receive-queue.
     *
     *  @param tcb The TCB reference of the thread to enqueue.
     */
    void enqueue_recv(tcb_t * tcb);

    /**
     *  Dequeue argument thread from this end-point's send-queue
     *
     *  @param tcb The TCB reference of the thread to dequeue.
     */
    void dequeue_recv(tcb_t * tcb);

private:
    /**
     *  Get the send queue syncpoint.
     */
    syncpoint_t * get_send_syncpoint(void);

    /**
     *  Get the receive queue syncpoint.
     */
    syncpoint_t * get_recv_syncpoint(void);

    /** The sync-point used to back this end-points send queue. */
    syncpoint_t send_queue;

    /* The receive queue is only required for schedule inheritance. */
#if defined(CONFIG_SCHEDULE_INHERITANCE)

    /** The sync-point used to back this end-points receive queue. */
    syncpoint_t recv_queue;

#endif

    friend void mkasmsym(void);
};

INLINE tcb_t *
endpoint_t::get_send_head(void)
{
    return this->send_queue.get_blocked_head();
}

INLINE tcb_t *
endpoint_t::get_recv_head(void)
{
#if defined(CONFIG_SCHEDULE_INHERITANCE)
    return this->recv_queue.get_blocked_head();
#else
    return NULL;
#endif
}

INLINE syncpoint_t *
endpoint_t::get_send_syncpoint(void)
{
    return &send_queue;
}

#if defined(CONFIG_SCHEDULE_INHERITANCE)
INLINE syncpoint_t *
endpoint_t::get_recv_syncpoint(void)
{
    return &recv_queue;
}
#endif

#endif /* __ENDPOINT_H__ */
