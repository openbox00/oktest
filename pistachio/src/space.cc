/*
 * Description:   architecture independent parts of space_t
 */

#include <l4.h>
#include <debug.h>
#include <kmemory.h>
#include <kmem_resource.h>
#include <space.h>
#include <tcb.h>
#include <schedule.h>
#include <syscalls.h>
#include <threadstate.h>
#include <arch/syscalls.h>
#include <cache.h>
#include <kernel/arch/special.h>
#include <kernel/bitmap.h>
#include <linear_ptab.h>
#include <kernel/generic/lib.h>
#include <smallalloc.h>
#include <phys_segment.h>

space_t * global_spaces_list = NULL;
spinlock_t spaces_list_lock;

/* Table containing mappings from spaceid_t to space_t* */
spaceid_lookup_t space_lookup;

void SECTION(SEC_INIT) init_spaceids(word_t max_spaceids,
                                     kmem_resource_t *kresource)
{
    void * new_table;
    ASSERT(ALWAYS, max_spaceids >= 1);

    new_table = kresource->alloc(kmem_group_spaceids,
                                 sizeof(space_t*) * max_spaceids, true);
    ASSERT(ALWAYS, new_table);

    space_lookup.init(new_table, max_spaceids);
    get_kernel_space()->set_space_id(spaceid(~0UL));
}

/**
 * Handle a pagefault as appropriate.
 * For a user fault this means a pagefault IPC, for a kernel fault this
 * means syncing the kernel space or mapping the dummy UTCB.
 *
 * This is a control function for a user fault, so the continuation will be
 * activated instead of returning. However for a kernel fault this is not a
 * control function, so the continuation will be ignored and a normal return
 * will be performed.
 *
 * @param addr The address of the fault
 * @param ip The address of the instructin that caused the fault
 * @param access The type of fault (RWX)
 * @param kernel Whether the fault came from kernel or user mode
 * @param continuation The continuation to activate upon completion
 * for a user fault, ignored for a kernel fault
 */
void
generic_space_t::handle_pagefault(addr_t addr, addr_t ip, access_e access,
                                  bool kernel, continuation_t continuation)
{
    tcb_t * current = get_current_tcb();

    if (EXPECT_TRUE(!kernel)) {
        // if we have a user fault we may have a stale partner
        current->set_partner(NULL);

        current->send_pagefault_ipc (addr, ip, access, continuation);
    }
    else {
        /* kernel fault */
        bool user_area = is_user_area(addr);

        if (user_area && current->user_access_enabled())
        {
            TCB_SYSDATA_USER_ACCESS(current)->fault_address = addr;
            continuation_t cont = current->user_access_continuation();
            /* Disable user address access */
            current->clear_user_access();

            ACTIVATE_CONTINUATION(cont);
            NOTREACHED();
        }

        if (!user_area) {
            if (sync_kernel_space(addr)) {
                return;
            }
        }
        else {
            enter_kdebug("kpf");
        }
    }

    enter_kdebug("unhandled pagefault");

    if (current == get_idle_tcb()) {
        panic("Unhandled pagefault in idle thread.");
    }

    get_current_scheduler()->
        deactivate_sched(current, thread_state_t::halted,
                         current, (continuation_t)(-1),
                         scheduler_t::sched_default);
    NOTREACHED();
}



SYS_SPACE_CONTROL (spaceid_t space_id, word_t control, clistid_t clist_id,
                   fpage_t utcb_area, word_t space_resources)
{
    LOCK_PRIVILEGED_SYSCALL();
    continuation_t continuation = ASM_CONTINUATION;
    space_t * space;
    clist_t * clist;

    tcb_t *current = get_current_tcb();

    // Check for valid control word
    if ( (control & SPACE_CONTROL_DELETE) &&
            (control & (SPACE_CONTROL_NEW | SPACE_CONTROL_RESOURCES)) )
    {
        current->set_error_code (EINVALID_PARAM);
        goto error_out;
    }

    if (EXPECT_FALSE ( (control & SPACE_CONTROL_KRESOURCES_ACCESSIBLE) &&
          !(control & SPACE_CONTROL_NEW)))
    {
        current->set_error_code (EINVALID_PARAM);
        goto error_out;
    }

    space = get_current_space()->lookup_space(space_id);

    if (control & SPACE_CONTROL_NEW)
    {
        prio_t prio;

        /* Ensure the space does not already exist. */
        if (EXPECT_FALSE( space != NULL ))
        {
            current->set_error_code (EINVALID_SPACE);
            goto error_out;
        }

        /*
         * Explicitly check id-range as lookup_space() could have failed due
         * to invalid id instead of non-existant space.
         */
        if (EXPECT_FALSE(!get_current_space()->space_range.is_valid(
                        space_id.get_spaceno()))) {
            current->set_error_code (EINVALID_SPACE);
            goto error_out;
        }

        /* Get the clist */
        clist = get_current_space()->lookup_clist(clist_id);
        if (EXPECT_FALSE( clist == NULL ))
        {
            current->set_error_code (EINVALID_CLIST);
            goto error_out;
        }

        /*
         * Check the specified priority is not higher than the maximum priority
         * allowed for the creator.
         */
        prio = (prio_t)(SPACE_CONTROL_PRIO(control) & 0xff);
        if (EXPECT_FALSE( prio > get_current_space()->get_maximum_priority() ))
        {
            current->set_error_code (EINVALID_PARAM);
            goto error_out;
        }

        /*
         * Space does not exist.  Create it.
         */

        /* @todo FIXME: The is_user_area should probably be a static
         * function, for now though we call it on a space that we know
         * exists, e.g: kernel space. This should really be refactored
         * though, so that control word is handled before the space is
         * init, then it makes sense for is_user_area to be a class
         * method - ???.
         */

        bool utcb_area_invalid;

#ifdef NO_UTCB_RELOCATE    /* UTCB is kernel allocated */
        utcb_area_invalid = (utcb_area.raw != 0);
#else   /* UTCB is user allocated */
        utcb_area_invalid = (UTCB_SIZE > utcb_area.get_size()) ||
                  (!get_kernel_space()->is_user_area(utcb_area));
#endif

        if (EXPECT_FALSE(utcb_area_invalid)) {
            /* Invalid UTCB area */
            current->set_error_code (EUTCB_AREA);
            goto error_out;
        }

        kmem_resource_t *kresource = get_current_kmem_resource();

        /* ok, everything seems fine, now setup the space */
        space = allocate_space(kresource, space_id, clist);
        if (EXPECT_FALSE(space == NULL)) {
            /* Assert error code is set by allocate_space() */
            ASSERT(DEBUG, current->get_error_code() != EOK);
            goto error_out;
        }

        if (EXPECT_FALSE(space->init(utcb_area, kresource) == false)) {
            space->free(kresource);
            free_space(space);

            /* Assert error code is set */
            ASSERT(DEBUG, current->get_error_code() != EOK);
            goto error_out;
        }

        /* Set the maximum priority of the space */
        space->set_maximum_priority(prio);

        if (control & SPACE_CONTROL_KRESOURCES_ACCESSIBLE) {
            /* Copy the creators resource partitions */
            space->grant_access_to_kresources();
            space->mutex_range = get_current_space()->mutex_range;
            space->space_range = get_current_space()->space_range;
            space->clist_range = get_current_space()->clist_range;
            space->phys_segment_list = get_current_space()->phys_segment_list;
        } else {
            /* This space gets no resources */
            space->mutex_range = get_current_space()->mutex_range;
            space->space_range.set_range(0, 0);
            space->clist_range.set_range(0, 0);
            space->phys_segment_list = NULL;
        }
    }
    else if (control & SPACE_CONTROL_DELETE)
    {
        /* Check that space exists */
        if (EXPECT_FALSE ( space == NULL ))
        {
            get_current_tcb ()->set_error_code (EINVALID_SPACE);
            goto error_out;
        }

        /* Ensure that all threads have already been deleted */
        if (EXPECT_FALSE ( space->get_thread_count() != 0 ))
        {
            current->set_error_code (ESPACE_NOT_EMPTY);
            goto error_out;
        }

        kmem_resource_t *kresource = get_current_kmem_resource();
        space->free(kresource);
        free_space(space);

        /* space control returns here on delete */
        UNLOCK_PRIVILEGED_SYSCALL();
        return_space_control (1, 0, continuation);
    }

    if (control & SPACE_CONTROL_RESOURCES)
    {
        /* Check that space exists */
        if (EXPECT_FALSE ( space == NULL ))
        {
            get_current_tcb ()->set_error_code (EINVALID_SPACE);
            goto error_out;
        }

        word_t old_resources = space->space_control (space_resources);
        UNLOCK_PRIVILEGED_SYSCALL();
        return_space_control (1, old_resources, continuation);
    }

    /* Assert error code is not set */
    ASSERT(DEBUG, current->get_error_code() == EOK);
    UNLOCK_PRIVILEGED_SYSCALL();
    return_space_control (1, 0, continuation);

error_out:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_space_control (0, 0, continuation);
}


/**
 * allocate_space: allocates a new space_t
 */
space_t * allocate_space(kmem_resource_t *res, spaceid_t space_id, clist_t *clist)
{
    space_t * space = (space_t *) res->alloc(kmem_group_space, true);
    if (EXPECT_FALSE(!space)) {
        get_current_tcb()->set_error_code(ENO_MEM);
        return NULL;
    }
    if (!space->allocate_page_directory(res)) {
        res->free(kmem_group_space, space);
        get_current_tcb()->set_error_code(ENO_MEM);
        return NULL;
    }

    space->set_kmem_resource(NULL);
    space->enqueue_spaces();

    space->set_space_id(space_id);
    get_space_list()->add_space(space_id, space);

    space->init_security(clist);

    return space;
}

/**
 * free_space: frees a previously allocated space
 */
void free_space(space_t * space)
{
    ASSERT(DEBUG, space);

    kmem_resource_t *kresource = get_current_kmem_resource();

    //TRACEF("Freeing %d\n", space->get_space_id());
    space->dequeue_spaces();
    space->free_page_directory(kresource);
    space->free_security();

    get_space_list()->remove_space(space->get_space_id());
    kresource->free(kmem_group_space, space);
}

void generic_space_t::free(kmem_resource_t *kresource)
{
    free_utcb_area_memory();

    // Unmap everything, including UTCB
    fpage_t fp = fpage_t::complete();
    unmap_fpage(fp, true, kresource);

#ifdef HAVE_ARCH_FREE_SPACE
    arch_free (kresource);
#endif
}

void generic_space_t::free_utcb_area_memory()
{
    /* ARMv5 frees its UTCBs as the threads are freed in free_thread_resources
     * This is feasible for ARM as it needs a UTCB bitmap anyway
     */
#if !defined(ARCH_ARM) || (CONFIG_ARM_VER >= 6)
    /* Unmap and free the UTCBs.. walk ptab, to see if any pages are mapped */
    fpage_t utcb_area = get_utcb_area();
    pgent_t::pgsize_e size = pgent_t::size_max;
    addr_t utcb = utcb_area.get_base();

    while (utcb_area.is_range_overlapping(utcb, addr_offset (utcb, page_size(UTCB_AREA_PGSIZE))))
    {
        pgent_t * pgent = this->pgent(0)->next(this, size, page_table_index(size, utcb));

        while (size > UTCB_AREA_PGSIZE && pgent->is_valid(this, size))
        {
            ASSERT(DEBUG, pgent->is_subtree(this, size));

            pgent = pgent->subtree(this, size);
            size--;
            pgent = pgent->next(this, size, page_table_index(size, utcb));
        }

        if (pgent->is_valid(this, size))
        {
            free_utcb_page(pgent, size, utcb);
        }
        utcb = addr_offset (utcb, page_size(UTCB_AREA_PGSIZE));
        size = pgent_t::size_max;
    }
#endif
}

void generic_space_t::free_utcb_page (pgent_t * pg, pgent_t::pgsize_e pgsize, addr_t vaddr)
{
    addr_t kaddr;
    word_t pagesize = 1UL << page_shift(pgsize);

    /* make sure vaddr is page aligned */
    ASSERT(DEBUG, !((word_t)vaddr & (pagesize-1)));

    /* flush the page on architectures with a virtual cache
     * so that we don't corrupt the freelist
     */
    cache_t::invalidate_virtual_alias(vaddr, pagesize);

    kaddr = ram_to_virt(pg->address(this, pgsize));

    //TRACEF("Freeing Kernel memory %p %p\n", vaddr, kaddr);

    /* flush from TLB */
    flush_tlbent_local(get_current_space(), vaddr, page_shift(pgsize));

    /* remove mapping */
    pg->clear (this, pgsize, true, vaddr);

    /* XXX flush remote cpu */

    /* free physical memory */
    get_current_kmem_resource()->free(kmem_group_utcb, kaddr, pagesize);
}

/* Map a region 
 *
 * Uses max page size available, but does not map any 
 * excess memory (beyond smallest page size granularity)
 */
bool SECTION(SEC_INIT)
    map_region (space_t * space, word_t vaddr, word_t paddr, word_t size, word_t attr, word_t rwx, kmem_resource_t *kresource)
{
    phys_desc_t phys_desc;
    word_t supported_sizes = HW_VALID_PGSIZES;
    word_t pgsize, pagesize = 0;
    fpage_t fpg;

    //TRACEF("Map region %p -> %p %lx bytes\n", vaddr, paddr, size);
    /* page align all addresses and size */
    size = (word_t)addr_align_up((addr_t)(((word_t)vaddr) + size), page_size(pgent_t::size_min)) - vaddr;
    vaddr = (word_t)addr_align((addr_t)vaddr, page_size(pgent_t::size_min));
    paddr = (word_t)addr_align((addr_t)paddr, page_size(pgent_t::size_min));

    while (size != 0)
    {
        /* find pagesize to use for this portion of the mapping */
        for (pgsize = BITS_WORD; pgsize; pgsize--) {
            pagesize = 1UL << pgsize;
            // Ignore sizes not supported by the kernel
            if (!(pagesize & supported_sizes)) {
                continue;
            }

            // Must be less that the size we're trying to map
            if (size < pagesize) {
                continue;
            }

            // phys and virt addrs must be aligned
            if (paddr & (pagesize-1)) {
                continue;
            }
            if (vaddr & (pagesize-1)) {
                continue;
            }
            break;
        }

        fpg.set(vaddr, pgsize, false, false, false);
        fpg.set_rwx(rwx);

        phys_desc.set_base(paddr);
        phys_desc.set_attributes(attr);

        //TRACEF("Map page %p -> %p %lx bytes\n", vaddr, paddr, page_size(pgsize));
        get_current_tcb()->sys_data.set_action
            (tcb_syscall_data_t::action_map_control);
        if (!space->map_fpage(phys_desc, fpg, kresource))
        {
            return false;
        }

        paddr += pagesize;
        vaddr += pagesize;
        size  -= pagesize;
    }
    return true;
}

clist_t * generic_space_t::lookup_clist(clistid_t clist_id)
{
    // Check privilege
    if (EXPECT_FALSE(!clist_range.is_valid(clist_id.get_clistno()))) {
        return NULL;
    } else {
        return get_clist_list()->lookup_clist(clist_id);
    }
}

space_t * generic_space_t::lookup_space(spaceid_t space_id)
{
    // Check privilege
    if (EXPECT_FALSE(!space_range.is_valid(space_id.get_spaceno()))) {
        return NULL;
    } else {
        return get_space_list()->lookup_space(space_id);
    }
}

mutex_t * generic_space_t::lookup_mutex(mutexid_t mutex_id)
{
    // Check privilege
    if (EXPECT_FALSE(!mutex_range.is_valid(mutex_id.get_number()))) {
        return NULL;
    } else {
        return mutexid_table.lookup(mutex_id);
    }
}
