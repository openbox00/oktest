/*
 * Description:   generic IPC path
 */
#include <l4.h>
#include <tcb.h>
#include <schedule.h>
#include <ipc.h>
#include <arch/ipc.h>
#include <syscalls.h>
#include <smp.h>
#include <arch/syscalls.h>

/*
 * Enqueue 'to_tcb' (if it exists) and return from the IPC.
 */
NORETURN static void
enqueue_tcb_and_return(tcb_t *current, tcb_t *to_tcb,
        continuation_t continuation)
{
    scheduler_t * scheduler = get_current_scheduler();
    if (to_tcb) {
        scheduler->activate_sched(to_tcb, thread_state_t::running,
                                  current, continuation,
                                  scheduler_t::sched_default);
    } else {
        ACTIVATE_CONTINUATION(continuation);
    }
}

/**
 * Restart the IPC operation as destination thread is now ready to receive
 * Threads that block on sending (in polling state) have their continuation
 * set to this function
 *
 * This is a continuation function so it's parameters are in the TCB
 *
 * @param ipc_to The to thread id of the IPC
 * @param ipc_from The from thread id of the IPC
 */
static CONTINUATION_FUNCTION(restart_ipc)
{
    tcb_t * current = get_current_tcb();

    SYS_IPC_RESTART(TCB_SYSDATA_IPC(current)->to_tid,
                    TCB_SYSDATA_IPC(current)->from_tid,
                    TCB_SYSDATA_IPC(current)->ipc_return_continuation);
}


/**
 * This function checks whether the received message was an async IPC
 * and does the appropriate set up for return if it was.
 *
 * It then returns through TCB_SYSDATA_IPC(current)->ipc_return_continuation function
 */
extern "C" CONTINUATION_FUNCTION(check_async_ipc)
{
    tcb_t * current = get_current_tcb();
    if (EXPECT_TRUE(current->sent_from.is_nilthread() &&
                    (!current->get_tag().is_error())))
    {
        ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->ipc_return_continuation);
    }

    /* XXX VU: restructure switching code so that dequeueing
     * from wakeup is removed from critical path */
    ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->ipc_return_continuation);
}

/**
 * Handle IPC errors
 */
static void NORETURN
return_ipc_error_send(tcb_t *current, tcb_t *to_tcb, tcb_t* from_tcb, word_t error)
{
    current->set_error_code(IPC_SND_ERROR(error));
    current->set_tag(msg_tag_t::error_tag());
    current->sent_from = NILTHREAD;

    if (to_tcb) { to_tcb->unlock_read(); }
    if (from_tcb) { from_tcb->unlock_read(); }
    return_ipc();
}

/**
 * Handle IPC errors with wakeup to_tcb
 */
static void NORETURN
return_ipc_error_recv(tcb_t *current, tcb_t *to_tcb, tcb_t* from_tcb, word_t error)
{
    current->set_error_code(IPC_RCV_ERROR(error));
    current->set_tag(msg_tag_t::error_tag());
    current->sent_from = NILTHREAD;

    if (from_tcb) { from_tcb->unlock_read(); }
    enqueue_tcb_and_return(current, to_tcb,
            TCB_SYSDATA_IPC(current)->ipc_return_continuation);
    NOTREACHED();
}

 /**********************************************************************
 *
 *                          IPC syscall
 *
 **********************************************************************/

SYS_IPC(capid_t to_tid, capid_t from_tid)
{
}

void ipc(tcb_t *to_tcb, tcb_t *from_tcb, word_t wait_type)
{

    tcb_t * current = get_current_tcb();
    scheduler_t * scheduler = get_current_scheduler();
    scheduler->context_switch(current, from_tcb, thread_state_t::waiting_forever,thread_state_t::running,
                        TCB_SYSDATA_IPC(current)->ipc_return_continuation);
}
 
