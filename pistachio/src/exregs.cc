/*
 * Description:   Iplementation of ExchangeRegisters()
 */
#include <l4.h>
#include <arch/syscalls.h>
#include <schedule.h>

#include <arch/tcb.h>

void handle_ipc_error (void);
void thread_return (void);

CONTINUATION_FUNCTION(finish_exchange_registers);

CONTINUATION_FUNCTION(finish_perform_exreg);

static void perform_exregs (tcb_t *tcb, word_t *control, word_t *usp,
                            word_t *uip, word_t *uflags, word_t *uhandle,
                            tcb_t *source, tcb_t *copy_src_locked)
{
    word_t ctrl = *control;

    tcb_t * current = get_current_tcb();

    if (ctrl & EXREGS_CONTROL_DELIVER)
    {
        TCB_SYSDATA_EXREGS(source)->sp          = tcb->get_user_sp();
        TCB_SYSDATA_EXREGS(source)->ip          = tcb->get_user_ip();
        TCB_SYSDATA_EXREGS(source)->flags       = tcb->get_user_flags();
        TCB_SYSDATA_EXREGS(source)->user_handle = tcb->get_user_handle();
    }
    TCB_SYSDATA_EXREGS(source)->new_control = 0;

    //get_current_scheduler()->pause(tcb);
    //tcb->unlock_read();

    if (ctrl & EXREGS_CONTROL_SP)
        tcb->set_user_sp ((addr_t)*usp);

    if (ctrl & EXREGS_CONTROL_IP)
        tcb->set_user_ip ((addr_t)*uip);

    if (ctrl & EXREGS_CONTROL_UHANDLE)
        tcb->set_user_handle (*uhandle);

    if (ctrl & EXREGS_CONTROL_FLAGS)
        tcb->set_user_flags (*uflags);

    if (ctrl & EXREGS_CONTROL_COPY_REGS)
    {
        tcb->copy_user_regs(copy_src_locked);
        //copy_src_locked->unlock_read();
    }

    if (ctrl & EXREGS_CONTROL_TLS) {
        (void)tcb->set_tls(&current->get_utcb()->mr[0]);
    }
    if (ctrl & EXREGS_CONTROL_MRS_TO_REGS)
    {
        source->copy_mrs_to_regs(tcb);
    }
    if (ctrl & EXREGS_CONTROL_REGS_TO_MRS)
    {
        source->copy_regs_to_mrs(tcb);
    }

    TCB_SYSDATA_EXREGS(current)->source = source;
    TCB_SYSDATA_EXREGS(source)->exreg_tcb = tcb;
    TCB_SYSDATA_EXREGS(source)->ctrl = ctrl;

    if (tcb->get_state().is_sending() || tcb->get_state().is_receiving())
    {
        word_t mask;

        if (tcb->get_state().is_sending()) {
            mask = EXREGS_CONTROL_SEND;
        } else {
            mask = EXREGS_CONTROL_RECV;
        }

       TCB_SYSDATA_EXREGS(source)->new_control |= mask;

        if (ctrl & mask) {
            tcb->unwind(tcb->get_partner());

            tcb->notify(handle_ipc_error);

            get_current_scheduler()->activate(tcb, thread_state_t::running);
            get_current_scheduler()->schedule(current, finish_perform_exreg,
                                              scheduler_t::sched_default);
        }
    }
    if (ctrl & (EXREGS_CONTROL_SEND | EXREGS_CONTROL_RECV)) {
        if (tcb->get_state().is_waiting_mutex()) {
            tcb->unwind(NULL);
            get_current_scheduler()->activate(tcb, thread_state_t::running);
            get_current_scheduler()->schedule(current, finish_perform_exreg,
                    scheduler_t::sched_default);
        }
    }

    //get_current_scheduler()->unpause(tcb);
    ACTIVATE_CONTINUATION(finish_perform_exreg);
}

CONTINUATION_FUNCTION(finish_perform_exreg)
{
    tcb_t * current = get_current_tcb();
    tcb_syscall_data_t::exregs_t *cur_info = TCB_SYSDATA_EXREGS(current);

    tcb_t * source = cur_info->source;
    tcb_syscall_data_t::exregs_t *src_info = TCB_SYSDATA_EXREGS(source);

    tcb_t * tcb = src_info->exreg_tcb;

    if (tcb->get_state().is_halted())
    {
        src_info->new_control |= EXREGS_CONTROL_HALT;
    }
    if (tcb->is_suspended()) {
        src_info->new_control |= EXREGS_CONTROL_HALT;
        src_info->new_control |= EXREGS_CONTROL_SUSPEND_FLAG;
    }

    if (src_info->ctrl & EXREGS_CONTROL_HALTFLAG) {
        if (src_info->ctrl & EXREGS_CONTROL_HALT) {
            if (tcb->get_state().is_running()) {
            }
        } else {
            if (tcb->get_state().is_halted() && !tcb->is_suspended()) {
            }
        }
    }

    if (src_info->ctrl & EXREGS_CONTROL_SUSPEND_FLAG) {
        if (src_info->ctrl & EXREGS_CONTROL_HALT) {
            if (!tcb->is_suspended()) {
                tcb->set_suspended(true);
                if (tcb->get_state().is_running()) {
                }
            }
        } else {
            if (tcb->is_suspended()) {
                tcb->set_suspended(false);
                tcb->set_post_syscall_callback(NULL);

                if (tcb->get_state().is_halted()) {
                }
            }
        }
    }

    ACTIVATE_CONTINUATION(finish_exchange_registers);
}

NORETURN SYS_EXCHANGE_REGISTERS (capid_t dest_tid, word_t control,
                        word_t usp, word_t uip, word_t uflags,
                        word_t uhandle)
{
    tcb_t * current = get_current_tcb();
    tcb_t * src_locked = NULL;
    continuation_t continuation = ASM_CONTINUATION;

    space_t * space = get_current_space();
    tcb_t *tcb_locked;

    if (dest_tid.is_myself()) {
        tcb_locked = acquire_read_lock_current(current);
    } else {
        tcb_locked = get_current_clist()->lookup_thread_cap_locked(dest_tid);
        if (tcb_locked == NULL) {
            tcb_locked = get_current_clist()->lookup_ipc_cap_locked(dest_tid);
            if (EXPECT_FALSE( tcb_locked && (space != tcb_locked->get_space())))
            {
                current->set_error_code (EINVALID_THREAD);
                goto error_out_locked;
            }
        }
    }

    current->sys_data.set_action(tcb_syscall_data_t::action_exregs);

    if (EXPECT_FALSE( tcb_locked == NULL ))
    {
        current->set_error_code (EINVALID_THREAD);
        goto error_out;
    }

    if ((control & (EXREGS_CONTROL_TLS|EXREGS_CONTROL_MRS_TO_REGS)) ==
        (EXREGS_CONTROL_TLS|EXREGS_CONTROL_MRS_TO_REGS)) {
        current->set_error_code(EINVALID_PARAM);
        goto error_out_locked;
    }

    if (control & EXREGS_CONTROL_COPY_REGS)
    {
        capid_t src_id = capid_t::capid(TYPE_CAP, control >> EXREGS_CONTROL_THREAD_SHIFT);
        src_locked = get_current_clist()->lookup_thread_cap_locked(src_id);

        if (src_locked == NULL) {
            src_locked = get_current_clist()->lookup_ipc_cap_locked(src_id);
            if (EXPECT_FALSE( src_locked && (space != src_locked->get_space())))
            {
                //src_locked->unlock_read();
                current->set_error_code (EINVALID_THREAD);
                goto error_out_locked;
            }
        }

        if (EXPECT_FALSE(src_locked == NULL)) {
            current->set_error_code (EINVALID_THREAD);
            goto error_out_locked;
        }
    }

    /* Save the return address for exregs */
    TCB_SYSDATA_EXREGS(current)->exreg_continuation = continuation;

    {
        perform_exregs (tcb_locked, &control, &usp, &uip, &uflags, &uhandle,
                        current, src_locked);
    }
    while(1);

error_out_locked:
    //tcb_locked->unlock_read();
error_out:
    return_exchange_registers (0, 0, 0, 0, 0,
            capid_t::nilthread(), 0, continuation);
}

CONTINUATION_FUNCTION(finish_exchange_registers)
{
    tcb_t * current = get_current_tcb();
    tcb_syscall_data_t::exregs_t *cur_info = TCB_SYSDATA_EXREGS(current);

    if (cur_info->ctrl & EXREGS_CONTROL_DELIVER)
    {
        return_exchange_registers(1,
                                  cur_info->new_control,
                                  (word_t)cur_info->sp,
                                  (word_t)cur_info->ip,
                                  cur_info->flags,
                                  capid_t::capid(0, 0),
                                  cur_info->user_handle,
                                  cur_info->exreg_continuation);
    }
    else
    {
        return_exchange_registers(1,
                                  cur_info->new_control,
                                  0,
                                  0,
                                  0,
                                  NILTHREAD,
                                  0,
                                  cur_info->exreg_continuation);
    }
}

