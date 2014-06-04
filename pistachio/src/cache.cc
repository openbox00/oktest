/*
 * Description:   Cache control implementation
 */

#include <l4.h>
#include <debug.h>
#include <config.h>
#include <tcb.h>
#include <capid.h>
#include <cache.h>
#include <schedule.h>
#include <space.h>
#include <spaceid.h>
#include <linear_ptab.h>
#include <arch/pgent.h>

extern spaceid_lookup_t space_lookup;

CONTINUATION_FUNCTION(range_flush);
CONTINUATION_FUNCTION(full_flush);
CONTINUATION_FUNCTION(range_flush_full);

SYS_CACHE_CONTROL (spaceid_t space_id, word_t control)
{
    tcb_t * current = get_current_tcb();

    current->sys_data.set_action(tcb_syscall_data_t::action_cache_control);

    TCB_SYSDATA_CACHE(current)->cache_continuation = ASM_CONTINUATION;
    NULL_CHECK(TCB_SYSDATA_CACHE(current)->cache_continuation);

    TCB_SYSDATA_CACHE(current)->space_id = space_id;

    /* setup continuation info */
    TCB_SYSDATA_CACHE(current)->ctrl = control;
    TCB_SYSDATA_CACHE(current)->op_index = 0;
    TCB_SYSDATA_CACHE(current)->op_offset = 0;

    if (TCB_SYSDATA_CACHE(current)->ctrl.operation() != cop_flush_range) {
        ACTIVATE_CONTINUATION(full_flush);
    } else {
        word_t num_ops = TCB_SYSDATA_CACHE(current)->ctrl.highest_item() + 1;

        if ((num_ops * 2) > IPC_NUM_MR)
        {
            current->set_error_code(EINVALID_PARAM);
            return_cache_control(0,
                                 TCB_SYSDATA_CACHE(current)->cache_continuation);
        }
        ACTIVATE_CONTINUATION(range_flush);
    }
}

/**
 * @brief The full cache (i, d, or both) is being flushed by explicit request
 */
CONTINUATION_FUNCTION(full_flush)
{
    tcb_t *current = get_current_tcb();
    int    retval = 1;

    preempt_enable(full_flush);

    word_t *offset = &(TCB_SYSDATA_CACHE(current)->op_offset);

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKINNER)
    {
        switch (TCB_SYSDATA_CACHE(current)->ctrl.operation())
        {
        case cop_flush_all:
            cache_t::flush_all_attribute(attr_clean_id, offset);
            break;
        case cop_flush_I_all:
            cache_t::flush_all_attribute(attr_clean_i, offset);
            break;
        case cop_flush_D_all:
            cache_t::flush_all_attribute(attr_clean_d, offset);
            break;

        default:
            current->set_error_code (EINVALID_PARAM);
            retval = 0;
        }
    }

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKOUTER)
    {
        switch (TCB_SYSDATA_CACHE(current)->ctrl.operation())
        {
        case cop_flush_all:
            arch_outer_cache_full_op(attr_clean_inval_id);
            break;
        case cop_flush_I_all:
            arch_outer_cache_full_op(attr_clean_inval_i);
            break;
        case cop_flush_D_all:
            arch_outer_cache_full_op(attr_clean_inval_d);
            break;

        default:
            current->set_error_code (EINVALID_PARAM);
            retval = 0;
        }

        arch_outer_drain_write_buffer();
    }

    preempt_disable();
    return_cache_control(retval,
                         TCB_SYSDATA_CACHE(current)->cache_continuation);
}

#define MAX_RANGE (1024 * 8)

/**
 * @brief Flush one or more explicit ranges as specified in the 
 *        Message Registers.
 */
CONTINUATION_FUNCTION(range_flush)
{
    tcb_t *current = get_current_tcb();
    spaceid_t space_id = TCB_SYSDATA_CACHE(current)->space_id;
    space_t *space;

    word_t op_last = TCB_SYSDATA_CACHE(current)->ctrl.highest_item();
    word_t retval = 1;

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKINNER)
    {
        /* Lookup target address space */
        if (EXPECT_TRUE(space_id.is_nilspace())) {
            /* implicit my_own_space reference */
            space = get_current_space();
        } else {
            /* lookup the space */
            space = get_current_space()->lookup_space(space_id);
        }

        /* Ensure that a valid space was found */
        if (EXPECT_FALSE(space == NULL)) {
            get_current_tcb()->set_error_code (EINVALID_SPACE);
            return_cache_control(0, TCB_SYSDATA_CACHE(current)->cache_continuation);
        }

        for (word_t i = TCB_SYSDATA_CACHE(current)->op_index; i <= op_last; i++)
        {
            word_t offset = TCB_SYSDATA_CACHE(current)->op_offset;

            word_t start = current->get_mr(i*2);
            word_t region = current->get_mr(i*2 + 1);;
            cacheattr_e attr = (cacheattr_e)(region >> (BITS_WORD-4));
            word_t size = region & ((1UL << (BITS_WORD-4)) - 1);

            if (offset) {
                size = size - offset;
                start = start + offset;
            } else {
            }

            if (EXPECT_FALSE(((word_t)attr & CACHE_ATTRIB_MASK_D_OP) ==
                             CACHE_ATTRIB_INVAL_D)) {
                /* XXX we don't support d-cache invalidate currently
                 * Invalidate MUST be privileged OR must do carefull checks so that
                 * you can't invalidate cache lines from a different address space
                 */
                // Check privilege
                if (! is_kresourced_space(get_current_space())) {
                    current->set_error_code (EINVALID_SPACE);
                    retval = 0;
                    goto error_out_range;
                }
            }

            if (cache_t::full_flush_cheaper(size)) {
                TCB_SYSDATA_CACHE(current)->op_offset = 0;
                ACTIVATE_CONTINUATION(range_flush_full);
            } else {
#ifdef CACHE_NEED_PGENT
                pgent_t * r_pg[pgent_t::size_max];
                pgent_t * pg;
                addr_t next;
#endif

                word_t blocksize;
                pgent_t::pgsize_e t_size = pgent_t::size_max;
                word_t pagesize = page_size(t_size);
#ifdef CACHE_NEED_PGENT
                pg = space->pgent(0)->next(space, t_size, page_table_index(t_size, (addr_t)start));
                next = addr_offset(addr_align((addr_t)start, pagesize), pagesize);
#endif

                /* Start flushing.  The range to be flushed is broken up into
                   smaller amounts to reduce the amount of repeated work 
                   when restarted after preemption */
                while (size > 0) {
                    /* lookup pagetable for start, cache upper levels if we can
                       for re-use on subsequent segments */

#ifdef CACHE_NEED_PGENT
                    if (!pg->is_valid(space, t_size)) {
                        if (size <= pagesize) {
                            /* If we get here, we're on an invalid page and
                               the remaining size doesn't get to the next page.
                               So we can give up */
                            break;
                        } else {
                            /* Jump to the next page in the hope that it may be
                               valid */
                            TCB_SYSDATA_CACHE(current)->op_offset += pagesize;
                            size -= pagesize;
                            start += pagesize;
                            goto calc_next;
                        }
                    } else if (pg->is_subtree(space, t_size)) {
                        t_size --;
                        r_pg[t_size] = pg->next(space, t_size+1, 1);
                        pagesize = page_size(t_size);
                        pg = pg->subtree(space, t_size+1)->next(space, t_size,
                                page_table_index(t_size, (addr_t)start));
                        next = addr_offset(addr_align((addr_t)start, pagesize), pagesize);
                        continue;
                    }
#endif
#ifdef CACHE_HANDLE_FAULT
                // assume minimum page size
                pagesize = page_size(pgent_t::size_min);
#endif

                    /* If we get here we have found the pagetable entry, (or we
                     * don't care about it) so it is valid to flush the address
                     * from the cache - but only for the current page */

                    blocksize = min(pagesize, MAX_RANGE);

#ifdef CACHE_NEED_PGENT
    again:
#endif
                    preempt_enable(range_flush);

                    word_t base = (word_t)addr_align((addr_t)start, blocksize);
                    word_t limit = base + blocksize;

                    if ((start+size) <= limit) {
                        blocksize = size;
                    } else {
                        blocksize = limit - start;
                    }

                    preempt_disable();
                    // do this early in fault case
                    TCB_SYSDATA_CACHE(current)->op_offset += blocksize;
#ifdef CACHE_HANDLE_FAULT
                    current->set_user_access(range_flush);
#endif
                    cache_t::flush_range_attribute(space,
                                                   (addr_t)start,
                                                   (addr_t)(start+blocksize),
                                                   attr);
#ifdef CACHE_HANDLE_FAULT
                    current->clear_user_access();
#endif
                    size -= blocksize;
                    start += blocksize;

#ifdef CACHE_NEED_PGENT
    calc_next:
                    if (start >= (word_t)next) {
                        if (page_table_index(t_size, (addr_t)start) == 0) {
                            do {
                                pg = r_pg[t_size];
                                t_size ++;
                                pagesize = page_size(t_size);
                            } while ((t_size < pgent_t::size_max) &&
                                    (page_table_index(t_size, (addr_t)start) == 0));
                        } else {
                            pg = pg->next(space, t_size, 1);
                        }
                        next = addr_offset(addr_align((addr_t)start, pagesize), pagesize);
                    } else {
                        if (size > 0) {
                            goto again;
                        }
                    }
#endif
                }
            }

            /*
             * Update for preemption purposes
             */
            TCB_SYSDATA_CACHE(current)->op_index = i + 1;
            TCB_SYSDATA_CACHE(current)->op_offset = 0;
        }
    }

    if (TCB_SYSDATA_CACHE(current)->ctrl.cache_level_mask() & CACHE_CTL_MASKOUTER)
    {
        /*
         * Outer cache flush always using physical address, therefore, no need to
         * use space_id and can be performed from another space as long as it is
         * privileged.
         */
        for (word_t i = TCB_SYSDATA_CACHE(current)->op_index; i <= op_last; i++)
        {
            word_t start = current->get_mr(i*2);
            word_t region = current->get_mr(i*2 + 1);;
            cacheattr_e attr = (cacheattr_e)(region >> (BITS_WORD-4));
            word_t size = region & ((1UL << (BITS_WORD-4)) - 1);

            // Check privilege
            if (! is_kresourced_space(get_current_space())) {
                current->set_error_code (EINVALID_SPACE);
                retval = 0;
                goto error_out_range;
            }

            preempt_enable(range_flush);
            arch_outer_cache_range_op((addr_t)start, size, attr);
            preempt_disable();
        }

        arch_outer_drain_write_buffer();
    }

error_out_range:
    return_cache_control(retval, TCB_SYSDATA_CACHE(current)->cache_continuation);
}

/**
 * @brief Jumping from a "range" flush to flushing the whole cache, then 
 *        resuming the iteration over MRs.
 */
CONTINUATION_FUNCTION(range_flush_full)
{
    tcb_t *current = get_current_tcb();

    word_t i = TCB_SYSDATA_CACHE(current)->op_index;
    word_t region = current->get_mr(i*2 + 1);
    cacheattr_e attr = (cacheattr_e)(region >> (BITS_WORD-4));

    preempt_enable(range_flush_full);
    cache_t::flush_all_attribute(attr, &TCB_SYSDATA_CACHE(current)->op_offset);
    preempt_disable();

    TCB_SYSDATA_CACHE(current)->op_index ++;
    TCB_SYSDATA_CACHE(current)->op_offset = 0;
    /* Continue with remaining range flushes */
    ACTIVATE_CONTINUATION(range_flush);
}
