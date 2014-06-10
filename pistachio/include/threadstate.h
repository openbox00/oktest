/*
 * Description:  thread state
 */
#ifndef __THREADSTATE_H__
#define __THREADSTATE_H__

/**
 * thread state definitions
 *
 * VU: they should go into the architecture specific part to allow
 *     special optimised encoding
 */
#define RUNNABLE_STATE(id)      (((id) << 1) | 0)
#define BLOCKED_STATE(id)       (((id) << 1) | 1)

/**
 * thread_state_t: current thread state
 */
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
        aborted                 = BLOCKED_STATE(7),
        xcpu_waiting_deltcb     = BLOCKED_STATE(8),
        xcpu_waiting_exregs     = BLOCKED_STATE(12),
        waiting_mutex           = BLOCKED_STATE(13)
    };

    /* constructors */
    thread_state_t(void) { this->state = aborted; };
    thread_state_t(thread_state_e state_) { this->state = state_; }
    thread_state_t(word_t state_) { this->state = (thread_state_e) state_; }

    /* state manipulation */
    bool is_runnable();
    bool is_sending()
        { return state == polling; }
    bool is_receiving()
        { return state == waiting_forever || state == waiting_notify; }
    bool is_halted()
        { return state == halted; }
    bool is_aborted()
        { return state == aborted; }
    bool is_running()
        { return state == running; }
    bool is_waiting()
        { return state == waiting_forever; }
    bool is_waiting_notify()
        { return state == waiting_notify; }
    bool is_waiting_forever()
        { return state == waiting_forever; }
    bool is_polling()
        { return state == polling; }
    bool is_xcpu_waiting()
        { return state == xcpu_waiting_deltcb ||
              state == xcpu_waiting_exregs; }
    bool is_waiting_mutex()
        {return state == waiting_mutex;}

    /* operators */
    bool operator == (const thread_state_t thread_state) const
        {
            return this->state == thread_state.state;
        }

    bool operator == (const thread_state_e state_) const
        {
            return this->state == state_;
        }

    bool operator != (const thread_state_t thread_state) const
        {
            return this->state != thread_state.state;
        }

    bool operator != (const thread_state_e state_) const
        {
            return this->state != state_;
        }

    operator word_t()
        {
            return (word_t)(long)this->state;
        }

    /* debugging */
    const char * string (void)
        {
#ifdef CONFIG_DEBUG
            switch (state) {
            case running:               return "RUNNING";
            case waiting_forever:       return "WAIT_FOREVER";
            case waiting_notify:        return "WAIT_NOTIFY";
            case waiting_mutex:         return "WAIT_MUTEX";
            case polling:               return "POLLING";
            case halted:                return "HALTED";
            case aborted:               return "ABORTED";
            case xcpu_waiting_deltcb:   return "WAIT_XPU_DELTCB";
            case xcpu_waiting_exregs:   return "WAIT_XPU_EXREGS";
            default:                    return "UNKNOWN";
            }
#else
            return "";
#endif
        }

    thread_state_e state;
};

INLINE bool
thread_state_t::is_runnable()
{
    /* inverse logic - lowermost bit no set means runnable */
    return !((word_t)(long)this->state & 1);
}


#endif /* __THREADSTATE_H__ */
