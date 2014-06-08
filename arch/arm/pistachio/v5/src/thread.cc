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


#if defined(__GNUC__)
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
#elif defined(__RVCT_GNU__)
#include <asmsyms.h>

NORETURN __asm void asm_switch_to(continuation_t cont, word_t dest, word_t utcb_location)
{
    mov     r12,    #0xff000000
    str     r2,     [r12, #0xff0]
    orr     sp,     sp,    #STACK_TOP
    mov     r12,    #ARM_GLOBAL_BASE
    str     r1,     [r12, #OFS_GLOBAL_CURRENT_TCB]
    bx      r0
}
#elif defined(_lint)
void asm_switch_to(continuation_t cont, word_t dest, word_t utcb_location);
#else
#error "Unknown compiler"
#endif

void switch_from(tcb_t * current, continuation_t continuation)
{
    ASSERT(ALWAYS, current->ready_list.next == NULL);

    /* Save any resources held by this thread. */
    if (EXPECT_FALSE(current->resource_bits)) {
        current->resources.save(current);
    }

    /* Save the return continuation. */
    current->cont = continuation;
}

void switch_to(tcb_t * dest, tcb_t * schedule)
{
    ASSERT(ALWAYS, dest->ready_list.next == NULL);
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

#include <schedule.h>

#ifdef CONFIG_IPC_FASTPATH
extern "C"
void async_fp_helper(tcb_t * to_tcb, tcb_t * current)
{
#if defined(__GNUC__)
    word_t link;
    {
        register word_t r_current   ASM_REG("r9") = (word_t)current;
        register word_t r_to_tcb    ASM_REG("r2") = (word_t)to_tcb;
        register word_t r_from      ASM_REG("lr") = 0;
        /* to_tcb       = r2 */
        /* current      = r9 */
        __asm__ __volatile__ (
            "   .global async_fp_helper_asm             \n"
            "   .balign 32                              \n"
            "async_fp_helper_asm:                       \n"
            "   sub     sp,     sp,     #64             \n"
            : "+r" (r_to_tcb), "+r" (r_current), "+r" (r_from)
        );

        to_tcb = (tcb_t*)r_to_tcb;
        current = (tcb_t*)r_current;

        link = r_from;
    }
#else
    word_t link = (word_t) ASM_CONTINUATION;
#endif

    scheduler_t * scheduler = get_current_scheduler();
    scheduler->activate_sched(to_tcb, thread_state_t::running,
                              current, (continuation_t)link,
                              scheduler_t::sched_default);
}
#endif
