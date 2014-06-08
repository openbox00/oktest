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
#include <soc/arch/soc.h>
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

/* As on MIPS */

#define SYSCALL_SAVED_REGISTERS (EXCEPT_IPC_SYS_MR_NUM + 1)

extern "C" NORETURN void
send_exception_ipc( word_t exc_num, word_t exc_code,
        arm_irq_context_t *context, continuation_t continuation);


extern "C" NORETURN void undefined_exception(arm_irq_context_t *context)
{

    word_t instr;
    continuation_t continuation = ASM_CONTINUATION;

#ifdef CONFIG_ARM_THUMB_SUPPORT
    if (context->cpsr & CPSR_THUMB_BIT)
        instr = *(u16_t *)(PC(context->pc));
    else
#endif
        instr = *(word_t *)(PC(context->pc));

    if (EXPECT_FALSE((context->cpsr & CPSR_MODE_MASK) != CPSR_USER_MODE)) {
    }

    send_exception_ipc(1, instr, context, continuation);

}


extern "C" NORETURN void reset_exception(arm_irq_context_t *context)
{
    continuation_t continuation = ASM_CONTINUATION;

    kdebug_entry_t kdebug_entry = kdebug_entries.kdebug_entry;
    if (EXPECT_FALSE(kdebug_entry != NULL)) {
    }
    halt_user_thread(continuation);
}

extern "C" word_t sys_wbtest(word_t op, word_t* arg0, word_t* arg1, word_t* arg2);
extern "C" void sys_arm_misc(arm_irq_context_t *context)
{
    continuation_t continuation = ASM_CONTINUATION;

    switch (context->r12 & 0xff) {
#if defined(CONFIG_THREAD_NAMES) \
        || defined(CONFIG_SPACE_NAMES) \
        || defined(CONFIG_MUTEX_NAMES)
    case L4_TRAP_KSET_OBJECT_NAME:
        {
            char name[MAX_DEBUG_NAME_LENGTH];
            word_t type = context->r0;
            word_t id = context->r1;
            word_t *name_words = (word_t *)name;

            /* Copy the name from the user's registers. */
            name_words[0] = context->r2;
            name_words[1] = context->r3;
            name_words[2] = context->r4;
            name_words[3] = context->r5;
            name[MAX_DEBUG_NAME_LENGTH - 1] = '\0';

            /* Set the object's name. */
            (void)kdb_set_object_name((object_type_e)type, id, name);
            return;
        }
#endif /* CONFIG_THREAD_NAMES || CONFIG_SPACE_NAMES || CONFIG_MUTEX_NAMES */
    default:
        break;
    }

    /* XXX - should deliver this as a exception IPC */
    halt_user_thread(continuation);
}

CONTINUATION_FUNCTION(finish_exception_ipc);

extern "C" NORETURN void
send_exception_ipc( word_t exc_num, word_t exc_code,
        arm_irq_context_t * context, continuation_t continuation)
{
    tcb_t * current = get_current_tcb();
    msg_tag_t tag;

    if (current->get_exception_handler() == NULL) {
        ACTIVATE_CONTINUATION(continuation);
    }
    /* XXX we should check if the kernel faulted */

    if (current->resource_bits.have_resource(EXCEPTIONFP)) {
#ifdef CONFIG_ARM_THUMB_SUPPORT
        TRACEPOINT (EXCEPTION_IPC_SYSCALL,
                word_t instr;
                if (context->cpsr & CPSR_THUMB_BIT)
                instr = *(u16_t *)(PC(context->pc)-2);
                else
                instr = *(word_t *)(PC(context->pc)-4);
                printf ("EXCEPTION_IPC_SYSCALL: (%p) IP = %p (%p)\n",
                    current, PC(context->pc),
                    (void*)instr));
#else
#endif
        // Create the message tag.
        tag.set(EXCEPT_IPC_SYS_MR_NUM, EXCEPTION_TAG, true, true);
    } else {

        current->arch.exc_code = exc_code;
        // Create the message tag.
        tag.set(EXCEPT_IPC_GEN_MR_NUM, EXCEPTION_TAG, true, true);
    }
    current->set_exception_ipc(exc_num);

    /* We are just about to send an IPC. */
    current->sys_data.set_action(tcb_syscall_data_t::action_ipc);

    // Save message registers.
    current->save_state(1);

    /* don't allow receiving async */
    current->set_notify_mask(0);

    current->set_tag(tag);

    current->arch.misc.exception.exception_ipc_continuation = continuation;
    current->arch.misc.exception.exception_context = context;

    // Deliver the exception IPC.
    current->do_ipc(current->get_exception_handler(),
                    current->get_exception_handler(),
                    finish_exception_ipc);
}

CONTINUATION_FUNCTION(finish_exception_ipc)
{
    msg_tag_t tag;
    tcb_t * current = get_current_tcb();

    tag.raw = current->get_mr(0);

    // Alter the user context if necessary.
    if (EXPECT_TRUE(tag.is_error())) {
        kdebug_entry_t kdebug_entry = kdebug_entries.kdebug_entry;
        if (EXPECT_FALSE(kdebug_entry != NULL)) {
        }
        else {
        }
        halt_user_thread(current->arch.misc.exception.exception_ipc_continuation);
    }

    // Clean-up.
    current->restore_state(1);
    current->clear_exception_ipc();

    ACTIVATE_CONTINUATION(current->
                          arch.misc.exception.exception_ipc_continuation);
}

extern "C" void check_coproc_fault(arm_irq_context_t *context, word_t instr, addr_t faddr)
{
}

bool tcb_t::copy_exception_mrs_from_frame(tcb_t *dest)
{
    return true;
}

bool tcb_t::copy_exception_mrs_to_frame(tcb_t *dest)
{
    return true;
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

