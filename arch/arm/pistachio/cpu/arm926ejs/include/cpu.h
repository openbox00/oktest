/*
 * Description:   ARM926EJ CPU control functions
 */

#ifndef __ARCH__ARM__ARM926EJ__CPU_H_
#define __ARCH__ARM__ARM926EJ__CPU_H_

#include <kernel/arch/asm.h>
class arm_cpu
{
public:
    static inline void cli(void)
    {
        __asm__  __volatile__ (
            "   mrs     r0,     cpsr             ;"
            "   orr     r0,     r0,     #0xd0    ;"
            "   msr     cpsr_c, r0               ;"
#if defined(__GNUC__)
            ::: "r0", "memory"
#endif
        );
    }

    static inline void sti(void)
    {
        __asm__  __volatile__ (
            "   mrs     r0,     cpsr             ;"
            "   and     r0,     r0,     #0x1f    ;"
            "   msr     cpsr_c, r0               ;"
#if defined(__GNUC__)
            ::: "r0", "memory"
#endif
        );
    }

    static inline void sleep(void)
    {
        word_t zero = 0;
        /* Versatile cannot currently sleep as it's timers 
         * will be turned off and it will never wake up
         * so busy wait for now
         */
        while(1);
        __asm__  __volatile__ (
            "   mcr     p15, 0, "_(zero)", c7, c0, 4    ;" /* Wait for interrupt */
#if defined(__GNUC__)
            :: [zero] "r" (zero)
#endif
        );
    }
};

#endif /* __ARCH__ARM__ARM926EJ__CPU_H_ */
