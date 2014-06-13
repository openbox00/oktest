#ifndef __THREADSTATE_H__
#define __THREADSTATE_H__

#define RUNNABLE_STATE(id)      (((id) << 1) | 0)
#define BLOCKED_STATE(id)       (((id) << 1) | 1)

class thread_state_t
{
public:
    enum thread_state_e
    {
        running                 = RUNNABLE_STATE(1),
        waiting_forever         = BLOCKED_STATE(~0UL >> 1),
        waiting_notify          = BLOCKED_STATE(2),
        polling                 = BLOCKED_STATE(5),
        halted                  = BLOCKED_STATE(6),
        aborted                 = BLOCKED_STATE(7)
    };
    thread_state_t(void) { this->state = aborted; };
    thread_state_t(thread_state_e state_) { this->state = state_; }
    bool is_runnable();
    thread_state_e state;
};

INLINE bool
thread_state_t::is_runnable()
{
    return !((word_t)(long)this->state & 1);
}


#endif /* __THREADSTATE_H__ */
