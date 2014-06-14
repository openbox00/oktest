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

#endif /* ! ASSEMBLY */

#endif /* ! __ARM__CONTINUATION_H__ */
