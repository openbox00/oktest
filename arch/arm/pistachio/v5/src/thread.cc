/*
 * Description:   ARMv5 specific thread handling functions
 */

#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <schedule.h>
#include <arch/thread.h>
#include <arch/schedule.h>
#include <cpu/syscon.h>
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
    /* Save any resources held by this thread. */
    if (EXPECT_FALSE(current->resource_bits)) {
        current->resources.save(current);
    }

    /* Save the return continuation. */
    current->cont = continuation;
}

void switch_to(tcb_t * dest, tcb_t * schedule)
{
    space_t *dest_space = dest->get_space();

    /* Update the global schedule variable. */
    set_active_schedule(schedule);

    /* Restore any resources held by the current thread. */
    if (EXPECT_FALSE(dest->resource_bits)) {
        dest->resources.load(dest);
    }
    /* Activate new FASS domain. */
    if (EXPECT_TRUE(dest_space != get_current_tcb()->get_space())) {
        get_arm_fass()->activate_domain(dest_space);
        /* Set PID of new space */
        if (dest_space) {
            get_arm_globals()->current_clist = dest_space->get_clist();
            write_cp15_register(C15_pid, C15_CRm_default, C15_OP2_default,
                    dest_space->get_pid() << 25);
        }
    }

    /* Perform the context switch for real. */
    asm_switch_to(dest->cont, (word_t)dest, dest->get_utcb_location());

    while(1);
}

