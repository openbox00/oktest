#include <l4.h>
#include <arch/syscalls.h>
#include <soc/soc.h>
#include <tcb.h>

SYS_PLATFORM_CONTROL(plat_control_t control, word_t param1,
                     word_t param2, word_t param3)
{
    LOCK_PRIVILEGED_SYSCALL();
    continuation_t continuation = ASM_CONTINUATION;
    tcb_t *current = get_current_tcb();
    word_t ret;

    if (EXPECT_FALSE(platcontrol_highest_item(control) >= IPC_NUM_MR))
    {
        current->set_error_code (EINVALID_PARAM);
        goto error_out;
    }

    /*
     * Check if thread may call platform control.
     */
    if (EXPECT_TRUE( get_current_space()->may_plat_control() ))
    {
        current = acquire_read_lock_current(current);
        if (EXPECT_FALSE(current == NULL)) {
            /* we are about to be deleted */
            get_current_tcb()->set_error_code(EINVALID_THREAD);
            goto error_out;
        }
        /* Call platform control */
        UNLOCK_PRIVILEGED_SYSCALL();
        ret = soc_do_platform_control((tcb_h)current, control, param1, param2, param3,
                continuation);
        LOCK_PRIVILEGED_SYSCALL();
        current->unlock_read();
        if (ret == 0) {
            /* error should be set in do_platform_control() */
            goto error_out;
        }
        UNLOCK_PRIVILEGED_SYSCALL();
        return_platform_control(ret, continuation);
    }

    /* Error case, thread has no privilege */
    current->set_error_code (EINVALID_SPACE);

error_out:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_platform_control(0, continuation);
}

