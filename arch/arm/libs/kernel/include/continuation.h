/**
 *  * @file
 *   * @brief The ARM-specific bits of working with continuation functions.
 *    */
#ifndef __ARM__CONTINUATION_H__
#define __ARM__CONTINUATION_H__

#include <kernel/arch/config.h>
/* Define Stack and Continuation handling functions */

#if !defined(ASSEMBLY)
#include <kernel/arch/asm.h>
#define ACTIVATE_CONTINUATION(continuation)     \
    do {                \
        __asm__ __volatile__ (                                          \
                "       orr     sp,     sp,     %1              \n"     \
                "       mov     pc,     %0                      \n"     \
                :                                                       \
                : "r" ((word_t)(continuation)),                         \
                  "i" (STACK_TOP)                                       \
                : "sp", "memory"                                        \
                );                                                      \
        while(1);                                                       \
    } while(false)


NORETURN INLINE void activate_function(word_t argument0, word_t argument1, word_t argument2, word_t function)
{
    register word_t arg0    ASM_REG("r0") = argument0;
    register word_t arg1    ASM_REG("r1") = argument1;
    register word_t arg2    ASM_REG("r2") = argument2;

    __asm__ __volatile__ (
        CHECK_ARG("r0", "%0")
        CHECK_ARG("r1", "%1")
        CHECK_ARG("r2", "%2")
        "       orr     sp,     sp,     %3              \n"
        "       mov     pc,     %4                      \n"
        :: "r" (arg0), "r" (arg1), "r" (arg2), "i" (STACK_TOP),
           "r" (function)
    );
    while (1);
}

#endif /* ! ASSEMBLY */

#endif /* ! __ARM__CONTINUATION_H__ */
