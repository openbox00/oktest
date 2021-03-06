#ifndef __GLUE__V4_ARM__KTCB_H__
#define __GLUE__V4_ARM__KTCB_H__

#include <arch/thread.h>

class arch_ktcb_t {
public:
    typedef union {
        struct {
            arm_irq_context_t * exception_context;
            continuation_t exception_ipc_continuation;
            continuation_t exception_continuation;
        } exception;

        struct {
            continuation_t handle_timer_interrupt_continuation;
            continuation_t arm_irq_continuation;
            word_t irq_number;
        } irq;

        struct {
            continuation_t page_fault_continuation;
            addr_t fault_addr;
        } fault;
    } ktcb_misc;

public:
    /* do not delete this STRUCT_START_MARKER */
    arm_irq_context_t context;
    ktcb_misc misc;
    word_t exc_code, exc_num;
    /* do not delete this STRUCT_END_MARKER */
};



#endif /* __GLUE__V4_ARM__KTCB_H__ */
