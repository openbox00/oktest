/*
 * Description:   ARM specific thread handling functions
 */

#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <arch/thread.h>
#include <cpu/syscon.h>

CONTINUATION_FUNCTION(do_ipc_helper)
{
    tcb_t * current = get_current_tcb();
    ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->do_ipc_continuation);
}
/**
 * Setup TCB to execute a function when switched to
 * @param func pointer to function
 *
 * The old stack state of the TCB does not matter.
 */
void tcb_t::create_startup_stack (void (*func)())
{
    init_stack();

    notify(func);

    arm_irq_context_t *context = &(arch.context);

    /* set user mode */
    context->cpsr = CPSR_USER_MODE;
}

