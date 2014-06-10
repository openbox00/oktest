#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

class tcb_t;

class endpoint_t
{
    friend class kdb_t;
public:
    tcb_t * get_send_head(void);
    tcb_t * get_recv_head(void);

private:
    syncpoint_t * get_send_syncpoint(void);
    syncpoint_t * get_recv_syncpoint(void);
    syncpoint_t send_queue;
    syncpoint_t recv_queue;
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
    return NULL;
}

INLINE syncpoint_t *
endpoint_t::get_send_syncpoint(void)
{
    return &send_queue;
}

#endif /* __ENDPOINT_H__ */
