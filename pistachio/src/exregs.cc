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

    // Load return values before they are clobbered.
    if (ctrl & EXREGS_CONTROL_DELIVER)
    {
        TCB_SYSDATA_EXREGS(source)->sp          = tcb->get_user_sp();
        TCB_SYSDATA_EXREGS(source)->ip          = tcb->get_user_ip();
        TCB_SYSDATA_EXREGS(source)->flags       = tcb->get_user_flags();
        TCB_SYSDATA_EXREGS(source)->user_handle = tcb->get_user_handle();
    }
    TCB_SYSDATA_EXREGS(source)->new_control = 0;

    get_current_scheduler()->pause(tcb);
    tcb->unlock_read();

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
        copy_src_locked->unlock_read();
    }

    if (ctrl & EXREGS_CONTROL_TLS) {
        (void)tcb->set_tls(&current->get_utcb()->mr[0]);
    }

    /* XXX FIXME This is undocumented (and often unimplemented) */
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
            /* Unwind might have added more threads to the ready queue. Perform
             * a full schedule. */
            get_current_scheduler()->schedule(current, finish_perform_exreg,
                                              scheduler_t::sched_default);
        }
    }

    /* Should we abort a mutex operation? */
    if (ctrl & (EXREGS_CONTROL_SEND | EXREGS_CONTROL_RECV)) {
        /* Is the thread in the middle of a muetx operation? */
        if (tcb->get_state().is_waiting_mutex()) {
            tcb->unwind(NULL);
            get_current_scheduler()->activate(tcb, thread_state_t::running);
            /* Unwind might have added more threads to the ready queue. Perform
             * a full schedule. */
            get_current_scheduler()->schedule(current, finish_perform_exreg,
                    scheduler_t::sched_default);
        }
    }

    get_current_scheduler()->unpause(tcb);
    ACTIVATE_CONTINUATION(finish_perform_exreg);
}

void halt_thread(tcb_t * tcb, continuation_t cont)
{
    // Halt a running thread
    get_current_scheduler()->pause(tcb);
    if (tcb->get_state().is_runnable()) {
        get_current_scheduler()->deactivate(tcb, thread_state_t::halted);
    } else {
        get_current_scheduler()->update_inactive_state(tcb, thread_state_t::halted);
    }

    /* Perform a full schedule if required. */
    if (tcb == get_current_tcb()) {
        get_current_scheduler()->schedule(tcb, cont, scheduler_t::sched_default);
    } else {
        tcb->release();
    }
}

/*
 * Resume the given thread from its halted state.
 */
void resume_thread(tcb_t * tcb, continuation_t cont)
{
    get_current_scheduler()->activate(tcb, thread_state_t::running);
    get_current_scheduler()->schedule(get_current_tcb(), cont, scheduler_t::sched_default);
}

/*
 * Halt the current TCB.
 *
 * This is called after a thread that the user suspends is in the
 * middle of a system call. When the thread is just about to
 * enter back into userspace this function is called.
 */
static void halt_current_thread(continuation_t cont)
{
    halt_thread(get_current_tcb(), cont);
}

/**
 * finishes up the exreg functions - specifically implements Halting and resuming of threads
 *
 * This is a continuation function so parameters are stored in the TCB
 *
 * @param exreg_source_tcb The source of the exreg call
 * @param perfrom_exreg_continuation The continuation to activate upon completion
 *
 * The following parameters are in the source TCB's misc.exregs structure
 * @param ctrl Exreg control parameter (in)
 * @param new_control Exreg control parameter (out)
 */
CONTINUATION_FUNCTION(finish_perform_exreg)
{
    tcb_t * current = get_current_tcb();
    tcb_syscall_data_t::exregs_t *cur_info = TCB_SYSDATA_EXREGS(current);

    tcb_t * source = cur_info->source;
    tcb_syscall_data_t::exregs_t *src_info = TCB_SYSDATA_EXREGS(source);

    tcb_t * tcb = src_info->exreg_tcb;

    /* Update return control flags. */
    if (tcb->get_state().is_halted())
    {
        src_info->new_control |= EXREGS_CONTROL_HALT;
    }
    if (tcb->is_suspended()) {
        src_info->new_control |= EXREGS_CONTROL_HALT;
        src_info->new_control |= EXREGS_CONTROL_SUSPEND_FLAG;
    }

    /* Check if we should resume a halted/resume a thread. */
    if (src_info->ctrl & EXREGS_CONTROL_HALTFLAG) {
        if (src_info->ctrl & EXREGS_CONTROL_HALT) {
            /* Halt a running thread if necessary. */
            if (tcb->get_state().is_running()) {
                halt_thread(tcb, finish_exchange_registers);
            }
            /* Halting a thread with ongoing kernel operations is not
             * supported. Such functionlity is implemented instead in
             * Suspend/Resume functionality.*/
        } else {
            /* If thread is halted, resume it. */
            if (tcb->get_state().is_halted() && !tcb->is_suspended()) {
                resume_thread(tcb, finish_exchange_registers);
            }
        }
    }

    /*
     * Determine if we should suspend the given thread. Suspending is
     * independent of halting to allow the kernel to be backwards compatible
     * with previous halting behaviour.
     *
     * Suspending a thread prevents a thread from executing any more userspace
     * instructions, but still complete pending system calls.
     */
    if (src_info->ctrl & EXREGS_CONTROL_SUSPEND_FLAG) {
        if (src_info->ctrl & EXREGS_CONTROL_HALT) {
            /* Suspend a running thread if necessary. */
            if (!tcb->is_suspended()) {
                tcb->set_suspended(true);
                tcb->set_post_syscall_callback(halt_current_thread);

                /* If the thread is running now, halt it. Otherwise the
                 * thread will be suspended next time just before it
                 * returns to userspace. */
                if (tcb->get_state().is_running()) {
                    halt_thread(tcb, finish_exchange_registers);
                }
            }
        } else {
            /* If thread is suspended, resume it. */
            if (tcb->is_suspended()) {
                tcb->set_suspended(false);
                tcb->set_post_syscall_callback(NULL);

                /* If the thread has been halted because of the suspend,
                 * resume execution. */
                if (tcb->get_state().is_halted()) {
                    resume_thread(tcb, finish_exchange_registers);
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
        /*
        * Only allow exregs if:
        *  - thread cap,
        *  or
        *  - ipc cap and in the same address space as target thread.
        */
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

    /* check that thread exists */
    if (EXPECT_FALSE( tcb_locked == NULL ))
    {
        current->set_error_code (EINVALID_THREAD);
        goto error_out;
    }

    /*
     * Check for exclusivity: both use MRs to pass in inputs,
     * this can't be possible.
     */
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
                src_locked->unlock_read();
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
        // Destination thread on same CPU.  Perform operation immediately.
        perform_exregs (tcb_locked, &control, &usp, &uip, &uflags, &uhandle,
                        current, src_locked);
    }
    while(1);

error_out_locked:
    tcb_locked->unlock_read();
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

