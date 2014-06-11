/*
 * Description:   ARM generic exception handling
 */

#include <l4.h>
#include <debug.h>
#include <space.h>
#include <tcb.h>
#include <ipc.h>
#include <arch/ipc.h>
#include <linear_ptab.h>
#include <kernel/generic/lib.h>
#include <schedule.h>
#include <arch/intctrl.h>

NORETURN INLINE void
halt_user_thread(continuation_t continuation)
{
    tcb_t * current = get_current_tcb();
    scheduler_t * scheduler = get_current_scheduler();

    scheduler->deactivate_sched(current, thread_state_t::halted,
                                current, continuation,
                                scheduler_t::sched_default);
}
#define SYSCALL_SAVED_REGISTERS (EXCEPT_IPC_SYS_MR_NUM + 1)

extern "C" NORETURN void reset_exception(arm_irq_context_t *context)
{
    continuation_t continuation = ASM_CONTINUATION;
    halt_user_thread(continuation);
}

void tcb_t::copy_user_regs(tcb_t *src)
{
    arm_irq_context_t * RESTRICT to = &arch.context;
    arm_irq_context_t * RESTRICT from = &(src->arch.context);

    to->r0 = from->r0;
    to->r1 = from->r1;
    to->r2 = from->r2;
    to->r3 = from->r3;
    to->r4 = from->r4;
    to->r5 = from->r5;
    to->r6 = from->r6;
    to->r7 = from->r7;
    to->r8 = from->r8;
    to->r9 = from->r9;
    to->r10 = from->r10;
    to->r11 = from->r11;
    to->r12 = from->r12;
    to->lr = from->lr;
    to->cpsr = from->cpsr;
}

void tcb_t::copy_regs_to_mrs(tcb_t *src)
{
    arm_irq_context_t * RESTRICT from = &(src->arch.context);
    word_t * RESTRICT mr = &this->get_utcb()->mr[0];

    mr[0] = from->r0;
    mr[1] = from->r1;
    mr[2] = from->r2;
    mr[3] = from->r3;
    mr[4] = from->r4;
    mr[5] = from->r5;
    mr[6] = from->r6;
    mr[7] = from->r7;
    mr[8] = from->r8;
    mr[9] = from->r9;
    mr[10] = from->r10;
    mr[11] = from->r11;
    mr[12] = from->r12;
    mr[13] = from->sp;
    mr[14] = from->lr;
    mr[15] = (word_t)src->get_user_ip();
    mr[16] = from->cpsr;
}

void tcb_t::copy_mrs_to_regs(tcb_t *dest)
{
    arm_irq_context_t * RESTRICT from = &(dest->arch.context);
    word_t * RESTRICT mr = &this->get_utcb()->mr[0];

    from->r0  = mr[0];
    from->r1  = mr[1];
    from->r2  = mr[2];
    from->r3  = mr[3];
    from->r4  = mr[4];
    from->r5  = mr[5];
    from->r6  = mr[6];
    from->r7  = mr[7];
    from->r8  = mr[8];
    from->r9  = mr[9];
    from->r10 = mr[10];
    from->r11 = mr[11];
    from->r12 = mr[12];
    from->sp  = mr[13];
    from->lr  = mr[14];
    dest->set_user_flags(mr[16]);
    dest->set_user_ip((addr_t)mr[15]);
}

