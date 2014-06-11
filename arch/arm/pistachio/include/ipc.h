/*
 * Description:   ARM IPC handling functions
 */
#ifndef __ARM__IPC_H__
#define __ARM__IPC_H__

void do_ipc_helper(void);
void restart_do_ipc(void);

INLINE void
tcb_t::do_ipc(tcb_t *to_tcb, tcb_t *from_tcb, continuation_t continuation)
{
    /* setup IPC restart/return info */
    this->sys_data.set_action(tcb_syscall_data_t::action_ipc);
    TCB_SYSDATA_IPC(this)->from_tid.set_raw((word_t)from_tcb);
    TCB_SYSDATA_IPC(this)->to_tid.set_raw((word_t)to_tcb);
    TCB_SYSDATA_IPC(this)->ipc_restart_continuation = restart_do_ipc;
    TCB_SYSDATA_IPC(this)->ipc_return_continuation = do_ipc_helper;

    /* Keep track of where we need to come back to. */
    TCB_SYSDATA_IPC(this)->do_ipc_continuation = continuation;

    to_tcb = acquire_read_lock_tcb(to_tcb);
    from_tcb = acquire_read_lock_tcb(from_tcb);

    /* Call sys_ipc with a custom continuation as the return address. */
    activate_function((word_t)to_tcb, (word_t)from_tcb, 3, (word_t)ipc);
}
#endif
