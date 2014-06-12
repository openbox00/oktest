/*
 * Description:   ARMv5 specific thread handling functions
 */
#include <tcb.h>
#include <schedule.h>
#include <arch/thread.h>
#include <schedule.h>

#define asm_switch_to(cont, dest, utcb_location)                            \
    do {                                                                    \
        __asm__ __volatile__ (                                              \
            "   mov     r12,    #0xff000000             \n"                 \
            "   str     %2,     [r12, #0xff0]           \n"                 \
            "   orr     sp,     sp,     %[st]           \n"                 \
            "   str     %[tcb], [%[glob], %[cur_tcb]]   \n"                 \
            "   mov     pc,     %0                      \n"                 \
            "1:                                         \n"                 \
            :                                                               \
            : "r" (cont), [tcb] "r" (dest), "r" (utcb_location),            \
              [st] "i" (STACK_TOP),                                         \
              [glob] "r" (get_arm_globals()),                               \
              [cur_tcb] "i" (offsetof(arm_globals_t, current_tcb))          \
            : "r12", "memory"                                               \
        );                                                                  \
        __asm__ __volatile__ ("" ::: "r0","r1","r2","r3","r6","r7" );       \
        __asm__ __volatile__ ("" ::: "r8","r9","r10","r11","memory" );      \
        while(1);                                                           \
    } while (0)

void switch_from(tcb_t * current, continuation_t continuation)
{
    current->cont = continuation;
}

void switch_to(tcb_t * dest, tcb_t * schedule)
{
    asm_switch_to(dest->cont, (word_t)dest, dest->get_utcb_location());
    while(1);
}

