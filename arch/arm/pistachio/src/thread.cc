/*
 * Description:   ARM specific thread handling functions
 */

#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <arch/thread.h>
#include <cpu/syscon.h>


/**
 * This function cleans up after an in-kernel
 * ipc and continues through the continuation
 * passed to do_ipc
 *
 * This function does not use any other stored
 * state from the TCB
 */

CONTINUATION_FUNCTION(do_ipc_helper)
{
    tcb_t * current = get_current_tcb();

#ifdef CONFIG_IPC_FASTPATH
    current->resources.clear_kernel_ipc( current );
    current->resources.clear_except_fp( current );
#endif

    ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->do_ipc_continuation);
}

CONTINUATION_FUNCTION(restart_do_ipc)
{
    tcb_t *current = get_current_tcb();
    tcb_t *to_tcb = (tcb_t*)TCB_SYSDATA_IPC(current)->to_tid.get_raw();
    tcb_t *from_tcb = (tcb_t*)TCB_SYSDATA_IPC(current)->from_tid.get_raw();

    to_tcb = acquire_read_lock_tcb(to_tcb);
    from_tcb = acquire_read_lock_tcb(from_tcb);

    /* Call sys_ipc with a custom continuation as the return address. */
    activate_function((word_t)to_tcb, (word_t)from_tcb, 3, (word_t)ipc);
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

