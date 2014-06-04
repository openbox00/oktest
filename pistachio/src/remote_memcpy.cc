/*
 * Description:   remote memory copy
 */

#include <l4.h>
#include <debug.h>
#include <config.h>
#include <tcb.h>
#include <capid.h>
#include <schedule.h>
#include <space.h>
#include <syscalls.h>
#include <remote_memcpy.h>
#include <linear_ptab.h>
#include <arch/user_access.h>

/*
 * This should be generic, but currently does not support all architectures.
 */
#if defined(CONFIG_REMOTE_MEMORY_COPY) && \
    (defined(CONFIG_ARCH_IA32) || defined(CONFIG_ARCH_ARM) || defined(CONFIG_ARCH_MIPS))
/** @todo FIXME: SMT/SMP safety - glee. */
static word_t memcpy_buf[REMOTE_MEMCPY_BUFSIZE/sizeof(word_t)];

static void memcpy_frombounce(tcb_t *tcb, word_t *addr, word_t size);
static void memcpy_tobounce(tcb_t *tcb, word_t *addr, word_t size);


/*
 * memory_copy_recover(): recover from invalid user memory access
 */
static CONTINUATION_FUNCTION(memory_copy_recover)
{
    tcb_t *current;
    word_t fault, addr;
    continuation_t cont;

    /*
     * We have lost all context at this point, but we need to return
     * how much we have actually copied.  To do this before we actually
     * do the copy we flag the starting address.  Then from the pagefault
     * address we can work out how many bytes we copied.
     */
    current = get_current_tcb();
    fault = (word_t)TCB_SYSDATA_USER_ACCESS(current)->fault_address;
    addr = (word_t)TCB_SYSDATA_MEMCPY(current)->copy_start;

    /*
     * At this state we could still be on the other guy's page table.
     * Switch the pagetable back to our own, just in case.
     */
    current->get_space()->activate(current);

    cont = TCB_SYSDATA_MEMCPY(current)->memory_copy_cont;

    /* signal that a mapping was missing */
    current->set_error_code(ENO_MEM);
    /* bye bye ... */
    UNLOCK_PRIVILEGED_SYSCALL();
    return_memory_copy(0, fault - addr, cont);
}

/*
 * memcpy_frombounce(): make memcpy from bounce buffer
 */
static void memcpy_frombounce(tcb_t *tcb, word_t *addr, word_t size)
{
    tcb_t *current;
    word_t *dest = (word_t*)&memcpy_buf[0];
    word_t i;

    current = get_current_tcb();
    tcb->get_space()->activate(tcb);
    current->set_user_access(memory_copy_recover);

    /* First copy in word size */
    for (i = 0; i < size / sizeof(word_t); i++) {
        user_write_word(addr++, *dest++);
    }

    current->clear_user_access();
}

/*
 * memcpy_tobounce(): make memcpy to bounce buffer
 */
static void memcpy_tobounce(tcb_t *tcb, word_t *addr, word_t size)
{
    tcb_t *current;
    word_t *src = (word_t*)&memcpy_buf[0];
    word_t i;

    current = get_current_tcb();
    tcb->get_space()->activate(tcb);
    current->set_user_access(memory_copy_recover);

    /* First copy in word size */
    for (i = 0; i < size / sizeof(word_t); i++) {
        *src++ = user_read_word(addr++);
    }

    current->clear_user_access();
}

static CONTINUATION_FUNCTION(memory_copy_loop)
{
    tcb_t *current, *from_tcb, *to_tcb;
    addr_t src, dest, orig_src, orig_dest;
    word_t size, remote_size, orig_size;
    word_t copy_size;
    word_t min_page_size = page_size(pgent_t::size_min);
    word_t min_page_mask = page_mask(pgent_t::size_min);
    continuation_t cont;
    LOCK_PRIVILEGED_SYSCALL();

    current = get_current_tcb();

    from_tcb =  TCB_SYSDATA_MEMCPY(current)->from_tcb;
    to_tcb =    TCB_SYSDATA_MEMCPY(current)->to_tcb;
    src =       TCB_SYSDATA_MEMCPY(current)->src;
    dest =      TCB_SYSDATA_MEMCPY(current)->dest;
    size =      TCB_SYSDATA_MEMCPY(current)->size;
    remote_size = TCB_SYSDATA_MEMCPY(current)->remote_size;
    orig_src =  TCB_SYSDATA_MEMCPY(current)->orig_src;
    orig_dest = TCB_SYSDATA_MEMCPY(current)->orig_dest;

    //printf("memcpy: from %p(%p, %lx), to %p(%p, %lx)\n", src, from_tcb, remote_size, dest, to_tcb, size);
    for (; remote_size > 0 && size > 0; /*null*/) {
        copy_size = min(remote_size, size);
        copy_size = min(REMOTE_MEMCPY_BUFSIZE, copy_size);

        /* Ensure that it does not span across the page boundary.
         *
         * If copy_size was across the page boundary and there was a
         * fault over the boundary, we must copy the data already
         * copied into the dest, where another page fault could occur.
         */
        word_t size_before_page_boundary =
            min_page_size - ((word_t)src & min_page_mask);
        copy_size = min(copy_size, size_before_page_boundary);

        TCB_SYSDATA_MEMCPY(current)->copy_start = orig_src;
        memcpy_tobounce(from_tcb, (word_t *)src, copy_size);
        TCB_SYSDATA_MEMCPY(current)->copy_start = orig_dest;
        memcpy_frombounce(to_tcb, (word_t *)dest, copy_size);

        /*
         * Put these adjustments within the loop so we can turn 
         * the preemption point on easily.
         */
        src = addr_offset(src, copy_size);
        dest = addr_offset(dest, copy_size);
        remote_size -= copy_size;
        size -= copy_size;
        /*
         * After a full copy give a chance for interrupts to flow through.
         * Stash the necessary data somewhere and then run the 
         * continuation.
         */
        TCB_SYSDATA_MEMCPY(current)->src = src;
        TCB_SYSDATA_MEMCPY(current)->dest = dest;
        TCB_SYSDATA_MEMCPY(current)->size = size;
        TCB_SYSDATA_MEMCPY(current)->remote_size = remote_size;
        UNLOCK_PRIVILEGED_SYSCALL();
        preempt_enable(memory_copy_loop);
        preempt_disable();
        LOCK_PRIVILEGED_SYSCALL();
    }
    /* switch back to current address space */
    if (to_tcb != current) {
        current->get_space()->activate(current);
    }

    cont = TCB_SYSDATA_MEMCPY(current)->memory_copy_cont;

    orig_size = TCB_SYSDATA_MEMCPY(current)->orig_size;
    /*
     * If there is still size remaining it means the copy has overflowed
     * the remote buffer.
     */
    if (size) {
        current->set_error_code(EINVALID_PARAM);
        UNLOCK_PRIVILEGED_SYSCALL();
        return_memory_copy(0, orig_size - size, cont);
    }

    UNLOCK_PRIVILEGED_SYSCALL();
    return_memory_copy(1, orig_size, cont);
}

/*
 * sys_memory_copy() system call
 */
SYS_MEMORY_COPY(capid_t remote, word_t local, word_t size,
                word_t direction)
{
    tcb_t *remote_tcb = NULL, *from_tcb, *to_tcb, *current;
    continuation_t cont = ASM_CONTINUATION;
    word_t remote_dir, remote_addr, remote_size, descidx;
    word_t src, dest;

    LOCK_PRIVILEGED_SYSCALL();

    current = get_current_tcb();

    /*
     * Step 1: make sure that the cap is valid
     */
    if (EXPECT_TRUE(remote.is_threadhandle())) {

        remote_tcb = lookup_tcb_by_handle_locked(remote.get_raw());

        if (EXPECT_FALSE(remote_tcb == NULL)) {
            current->set_error_code(EINVALID_CAP);
            goto error_out;
        }

        if (EXPECT_FALSE(!remote_tcb->get_state().is_waiting() ||
                         !remote_tcb->is_partner_valid() ||
                         remote_tcb->get_partner() != current)) {
            remote_tcb->unlock_read();
            current->set_error_code(EINVALID_PARAM);
            goto error_out;
        }
    } else {
        current->set_error_code(EINVALID_CAP);
        goto error_out;
    }
    /*
     * XXX 
     *
     * the lock should be held for the whole system call but the 
     * preemption isn't very well-defined yet.  This needs to be fixed
     * for SMT with preemption.
     */
    remote_tcb->unlock_read();

    /*
     * Step 2: make sure that the direction specified in the memory
     * descriptor allows copying in the specified direction
     */
    if (EXPECT_TRUE(!remote_tcb->get_tag().get_memcpy())) {
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }

    descidx = remote_tcb->get_tag().get_untyped() + 1/*tag*/;
    /* Check for message overflow */
    if (EXPECT_FALSE((descidx + 2) >= IPC_NUM_MR)) {
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }

    remote_addr = remote_tcb->get_mr(descidx);
    remote_size = remote_tcb->get_mr(descidx + 1);
    remote_dir = remote_tcb->get_mr(descidx + 2);

    if (EXPECT_FALSE(remote_dir == direction)) {
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }

#define WORD_MASK (sizeof(word_t) - 1)
    if ((local & WORD_MASK) || (size & WORD_MASK) ||
        (remote_addr & WORD_MASK) || (remote_size & WORD_MASK)) {
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }
#undef WORD_MASK

    /* 
     * check the start and end of the address to see if within
     * user area (check both to catch overflow bugs
     */
    if (!current->get_space()->is_user_area((addr_t)local) ||
        !current->get_space()->is_user_area((addr_t)(local + size - 1)) ||
        !remote_tcb->get_space()->is_user_area((addr_t)remote_addr) ||
        !remote_tcb->get_space()->is_user_area((addr_t)(remote_addr + 
        remote_size - 1))) {
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }

    /*
     * Step 3: enable the kernel user memory access feature and 
     * do the copy, while doing check to see if it will overflow
     * the bounds of the memory descriptor.  If it does then return
     * a short copy.
     */

    if (direction == (word_t) memcpy_dir_to) {
        src = remote_addr;
        dest = local;
        from_tcb = remote_tcb;
        to_tcb = current;
    } else if (direction == (word_t) memcpy_dir_from) {
        src = local;
        dest = remote_addr;
        from_tcb = current;
        to_tcb = remote_tcb;
    } else {
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }

    /*
     * Stash the data into the tcb of the thread so that
     * we can pull it out in case we need to do a preemption.
     */
    current->sys_data.set_action(tcb_syscall_data_t::action_remote_memcpy);
    TCB_SYSDATA_MEMCPY(current)->to_tcb = to_tcb;
    TCB_SYSDATA_MEMCPY(current)->from_tcb = from_tcb;
    TCB_SYSDATA_MEMCPY(current)->src = (addr_t)src;
    TCB_SYSDATA_MEMCPY(current)->dest = (addr_t)dest;
    TCB_SYSDATA_MEMCPY(current)->orig_src = (addr_t)src;
    TCB_SYSDATA_MEMCPY(current)->orig_dest = (addr_t)dest;
    TCB_SYSDATA_MEMCPY(current)->size = size;
    TCB_SYSDATA_MEMCPY(current)->orig_size = size;
    TCB_SYSDATA_MEMCPY(current)->remote_size = remote_size;
    TCB_SYSDATA_MEMCPY(current)->memory_copy_cont = cont;

    UNLOCK_PRIVILEGED_SYSCALL();
    ACTIVATE_CONTINUATION(memory_copy_loop);
    /*NOTREACHED*/

error_out:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_memory_copy(0, 0, cont);
}
#else
/*
 * sys_memory_copy() system call
 */
SYS_MEMORY_COPY(capid_t remote, word_t local, word_t size,
                word_t direction)
{
    continuation_t cont = ASM_CONTINUATION;
    get_current_tcb()->set_error_code(ENOT_IMPLEMENTED);
    return_memory_copy(0, 0, cont);
}
#endif    /* !CONFIG_REMOTE_MEMORY_COPY */
