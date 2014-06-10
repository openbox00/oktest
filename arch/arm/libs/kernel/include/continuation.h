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
#if defined (__GNUC__)
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


/* call a function with 2 arguments + a continuation so that the continuation can be retrieved by ASM_CONTINUATION */
NORETURN INLINE void call_with_asm_continuation(word_t argument0, word_t argument1, word_t continuation, word_t function)
{
    register word_t arg0    ASM_REG("r0") = argument0;
    register word_t arg1    ASM_REG("r1") = argument1;
    register word_t lr      ASM_REG("r14") = continuation;

    __asm__ __volatile__ (
        CHECK_ARG("r0", "%0")
        CHECK_ARG("r1", "%1")
        CHECK_ARG("lr", "%2")
        "       orr     sp,     sp,     %3              \n"
        "       mov     pc,     %4                      \n"
        :: "r" (arg0), "r" (arg1), "r" (lr), "i" (STACK_TOP),
        "r" (function)
    );
    while (1);
}

/* call a NORETURN function with 3 arguments + reset stack */
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

#elif defined (__RVCT_GNU__)

C_LINKAGE NORETURN void jump_and_set_sp(word_t ret, addr_t context);
C_LINKAGE NORETURN void call_with_asm_continuation(word_t argument0, word_t argument1, word_t continuation, word_t function);
C_LINKAGE NORETURN void activate_function(word_t argument0, word_t argument1, word_t argument2, word_t function);

#define TOP_STACK       ((addr_t)(__current_sp() | STACK_TOP))

#define ACTIVATE_CONTINUATION(continuation)     \
    jump_and_set_sp((word_t)(continuation), (addr_t)TOP_STACK)


#elif defined(_lint)
void jump_and_set_sp(word_t ret, addr_t context);
void call_with_asm_continuation(word_t argument0, word_t argument1, word_t continuation, word_t function);
void activate_function(word_t argument0, word_t argument1, word_t argument2, word_t function);
#define TOP_STACK       ((tcb_t **)0)
#define ACTIVATE_CONTINUATION(continuation) ((continuation_t) continuation)()
#if defined(__cplusplus)
class tcb_t;
#else
typedef struct tcb tcb_t;
#endif
INLINE tcb_t * get_current_tcb(void)
{
    return NULL;
}
#else
#error "Unknown compiler"
#endif

#endif /* ! ASSEMBLY */

#endif /* ! __ARM__CONTINUATION_H__ */
