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
        aborted                 = BLOCKED_STATE(7)
    };
    thread_state_t(void) { this->state = aborted; };
    thread_state_t(thread_state_e state_) { this->state = state_; }
    thread_state_e state;
};


#endif /* __THREADSTATE_H__ */
