/*
 * Description:   ARMv5 specific TCB handling functions
 */
#ifndef __ARM__V5__TCB_H__
#define __ARM__V5__TCB_H__

#include <arch/fass.h>
#include <arch/fass_inline.h>


#define asm_initial_switch_to(new_sp, new_pc)                               \
    __asm__ __volatile__ (                                                  \
        "    mov     sp,     %0              \n" /* load new stack ptr */   \
        "    mov     pc,     %1              \n" /* load new PC */          \
        :: "r" (new_sp), "r" (new_pc)                                       \
        : "memory")

INLINE void tcb_t::set_user_ip(addr_t ip)
{
    word_t new_pc = (word_t)ip;
    arm_irq_context_t *context = &arch.context;

    /* Keep exception frame type bit */
    if (context->pc & 1)
        new_pc |= 1;
    else
        new_pc &= ~1UL;

    context->pc = new_pc;
}

#endif /*__ARM__V5__TCB_H__*/
