/*
 * Description:   Preemption flags
 */
#ifndef __API__V4__PREEMPT_H__
#define __API__V4__PREEMPT_H__

PACKED(
    class preempt_flags_t
    {
    public:
        union {
            u8_t raw;
            struct {
                BITFIELD3 (u8_t,
                           : 5,
                           signaled : 1,
                           : 2
                          );
            } flags;
        };
    
        bool is_signaled()  { return flags.signaled; }
    }
);

#endif /* !__API__V4__PREEMPT_H__ */
