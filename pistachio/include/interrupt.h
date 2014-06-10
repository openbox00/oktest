/*
 * Description:  Interrupt handling and abstraction
 */
#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <arch/intctrl.h>

class irq_control_t
{
public:
    enum irq_op_e {
        op_register     = 0,
        op_unregister   = 1,
        op_ack          = 2,
        op_ack_wait     = 3
    };

    inline word_t get_count() { return x.count; }
    inline irq_op_e get_op() { return (irq_op_e)x.op; }
    inline word_t get_request() { return x.request; }
    inline word_t get_notify_bit() { return x.notify_bit; }

    inline word_t get_raw() { return raw; }

    void set_raw(word_t raw) { this->raw = raw; }

private:
    union {
        struct {
#if defined(CONFIG_IS_32BIT)
            BITFIELD5(word_t,
                    count   : 6,
                    op      : 2,
                    request : BITS_WORD - 14,
                            : 1,
                    notify_bit : 5);
#else
            BITFIELD4(word_t,
                    count   : 6,
                    op      : 2,
                    request : BITS_WORD - 14,
                    notify_bit : 6);
#endif
        } x;
        word_t raw;
    };
};

#ifdef CONFIG_MDOMAINS
/**
 * migration function for interrupt threads
 * deals with re-routing
 */
void migrate_interrupt_start (tcb_t * tcb);
void migrate_interrupt_end (tcb_t * tcb);
#endif

#endif /* __INTERRUPT_H__ */
