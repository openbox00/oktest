/*
 * Description:  Interrupt handling
 */

#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <space.h>
#include <soc/soc.h>
#include <soc/interface.h>
#include <interrupt.h>
#include <schedule.h>
#include <smp.h>
#include <atomic_ops/atomic_ops.h>


/*
 * Deliver notify bits to a handler thread.
 * If cont is NON-NULL, only one interrupt descriptors is pending
 * and only one handler will be woken up.
 */
extern "C"
bool kernel_deliver_notify(tcb_h handler, word_t notifybits, continuation_t cont)
{
    tcb_t *handler_tcb = (tcb_t*)handler;

    word_t bits = handler_tcb->add_notify_bits(notifybits);

async_wakeup_check:
    if (EXPECT_TRUE( (bits & handler_tcb->get_notify_mask()) &&
                ( handler_tcb->get_state().is_waiting_notify() ||
                  (handler_tcb->get_state().is_waiting() &&
                   ((word_t)handler_tcb->get_partner() == ~0UL)
                     ) ) ))
    {
        if (!handler_tcb->grab()) {
            okl4_atomic_barrier_smp();
            goto async_wakeup_check;
        }

        handler_tcb->sent_from = NILTHREAD;
        handler_tcb->set_tag(msg_tag_t::nil_tag()); /* clear tag of tcb */

        if (EXPECT_TRUE(cont)) {
            get_current_scheduler()->activate_sched(
                    handler_tcb, thread_state_t::running,
                    get_current_tcb(), cont,
                    scheduler_t::preempting_thread);
            NOTREACHED();
        } else {
            get_current_scheduler()->activate(handler_tcb, thread_state_t::running);
            return true;
        }
    }
    /* Do not wakeup destination */
    return false;
}

INLINE void setup_notify_return(tcb_t *tcb)
{
    word_t mask = tcb->get_notify_mask();
    word_t bits = tcb->sub_notify_bits(mask);

    tcb->set_tag(msg_tag_t::notify_tag());
    tcb->set_mr(1, bits & mask);
}

extern "C" CONTINUATION_FUNCTION(check_async_irq)
{
    tcb_t * current = get_current_tcb();

    setup_notify_return(current);
    return_interrupt_control(1, TCB_SYSDATA_IPC(current)->ipc_return_continuation);
}

SYS_INTERRUPT_CONTROL(capid_t tid, irq_control_t control)
{
    continuation_t continuation = ASM_CONTINUATION;

    tcb_t *handler;
    tcb_t *current = get_current_tcb();
    word_t ret;

    if (tid.is_nilthread()) {
        handler = acquire_read_lock_current(current);
    } else {
        handler = get_current_clist()->lookup_thread_cap_locked(tid);
    }

    /*
     * handler does not exist
     */
    if (EXPECT_FALSE(handler == NULL))
    {
        current->set_error_code(EINVALID_THREAD);
        goto error_out;
    }

    /* Check privilege */
    if ( EXPECT_FALSE( handler->get_space() != get_current_space() &&
                !is_kresourced_space(get_current_space()) ))
    {
        current->set_error_code (EINVALID_PARAM);
        goto error_out_locked;
    }

    switch (control.get_op()) {
    case irq_control_t::op_register:
    case irq_control_t::op_unregister: {
        soc_irq_action_e action = (control.get_op() == irq_control_t::op_register ?
                                   soc_irq_register :
                                   soc_irq_unregister);
        /* Call platform code to configure interrupts */
        ret = arch_config_interrupt(
                (struct irq_desc *)&current->get_utcb()->mr[0],
                control.get_notify_bit(),
                handler,
                handler->get_space(),
                action,
                handler->get_utcb());

        if (EXPECT_FALSE(ret)) {
            current->set_error_code(ret);
            goto error_out_locked;
        }
        handler->unlock_read();
        break;
    }
    case irq_control_t::op_ack:
    case irq_control_t::op_ack_wait:
        {
            word_t retval =
                arch_ack_interrupt((struct irq_desc *)&current->get_utcb()->mr[0],
                                  handler);
            if (retval) {
                current->set_error_code(retval);
                goto error_out_locked;
            }
            handler->unlock_read();
        }

        /* Determine if the user has requested to sleep until our next interrupt
         * arrives. */
        if (EXPECT_FALSE(control.get_op() == irq_control_t::op_ack)) {
            break;
        }

        /* Check to see if we have any pending notify bits: if so, we return
         * immeadiatly instead of sleeping. */
        if (EXPECT_TRUE( current->get_notify_bits() & current->get_notify_mask() ))
        {
            //printf("notify recieve (curr=%t)\n", current);
            setup_notify_return(current);
            return_interrupt_control(1, continuation);
        }

        /* Otherwise, sleep until we get our next interrupt. */
        current->sys_data.set_action(tcb_syscall_data_t::action_ipc);
        TCB_SYSDATA_IPC(current)->ipc_return_continuation = continuation;

        get_current_scheduler()->
            deactivate_sched(current, thread_state_t::waiting_notify,
                    current, check_async_irq,
                    scheduler_t::sched_default);
        break; // For completeness; never reached
    default:
        current->set_error_code (EINVALID_PARAM);
        goto error_out_locked;
    }

    /* No further operations to perform. */
    return_interrupt_control(1, continuation);

error_out_locked:
    handler->unlock_read();
error_out:
    return_interrupt_control(0, continuation);
}

