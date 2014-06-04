/*
 * Description: Space switch implementation.
 */

#include <l4.h>
#include <schedule.h>
#include <space.h>
#include <syscalls.h>
#include <tcb.h>
#include <capid.h>

SYS_SPACE_SWITCH(capid_t tid, spaceid_t space_id, word_t utcb_location)
{
    LOCK_PRIVILEGED_SYSCALL();
    continuation_t continuation = ASM_CONTINUATION;
    word_t retval = 0;

    tcb_t *current = get_current_tcb();

    space_t *new_space, *old_space;
    utcb_t *old_utcb;
    word_t old_utcb_location;
    utcb_t *new_utcb;
    tcb_t *tcb;

    if (tid.is_myself()) {
        tcb = acquire_read_lock_current(current);
    } else {
        tcb = get_current_clist()->lookup_thread_cap_locked(tid);
    }

    /* Check for valid thread id. */
    if (EXPECT_FALSE(tcb == NULL)) {
        current->set_error_code(EINVALID_THREAD);
        goto out1;
    }

    new_space = get_current_space()->lookup_space(space_id);
    /* Check for valid space id. */
    if (EXPECT_FALSE(new_space == NULL))
    {
        current->set_error_code(EINVALID_SPACE);
        goto out1_locked;
    }

    old_space = tcb->get_space();

    if (EXPECT_FALSE(!(is_kresourced_space(get_current_space()) &&
                    is_kresourced_space(new_space) &&
                    is_kresourced_space(old_space))))
    {
        /* Only allow space switch when all spaces involved have resources */
        current->set_error_code(EINVALID_SPACE);
        goto out1_locked;
    }

    /* Check for valid UTCB location. */
    if (EXPECT_FALSE(!new_space->check_utcb_location(utcb_location)))
    {
        current->set_error_code(EUTCB_AREA);
        goto out1_locked;
    }

    /* Work out where we're coming from. */
    UNLOCK_PRIVILEGED_SYSCALL();
    get_current_scheduler()->pause(tcb);
    LOCK_PRIVILEGED_SYSCALL();
    tcb->unlock_read();

    old_utcb = tcb->get_utcb();
    old_utcb_location = tcb->get_utcb_location();

    /* Try allocate new UTCB in new space */
    tcb->set_utcb_location(utcb_location);
    new_utcb = new_space->allocate_utcb(tcb,
                                        get_current_kmem_resource());

    if (EXPECT_FALSE(new_utcb == NULL)) {
        /* Back out any changes we've made. */
        tcb->set_utcb_location(old_utcb_location);
        current->set_error_code(ENO_MEM);
        goto out2;
    }

    /* Fix up the thread's space. */
    old_space->remove_tcb(tcb);
    tcb->set_space(new_space);
    new_space->add_tcb(tcb);

    /* Fix up the UTCB and copy across the old contents.  */
    tcb->set_utcb(new_utcb);
    new_utcb->copy(old_utcb);
    old_space->free_utcb(old_utcb);

    /* If the target is the current thread, switch to the new space now.  */
    if (tcb == current) {
        new_space->activate(tcb);
    }
    retval = 1;

out2:
    get_current_scheduler()->unpause(tcb);
    UNLOCK_PRIVILEGED_SYSCALL();
    return_space_switch(retval, continuation);

out1_locked:
    tcb->unlock_read();
out1:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_space_switch(retval, continuation);
}
