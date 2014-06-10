/*
 * Description: ARM space_t implementation.
 */

#include <l4.h>
#include <debug.h>                      /* for UNIMPLEMENTED    */
#include <linear_ptab.h>
#include <space.h>              /* space_t              */
#include <tcb.h>
#include <arch/pgent.h>
#include <arch/memory.h>
#include <config.h>
#include <arch/fass.h>
#include <cpu/syscon.h>
#include <kernel/arch/special.h>
#include <kernel/bitmap.h>
#include <kernel/generic/lib.h>
#include <kmem_resource.h>


#if UTCB_AREA_SECTIONS != CONFIG_MAX_SPACES
#error CONFIG_MAX_SPACES must be the same as UTCB_AREA_SECTIONS on the ARM architecture
#endif

/* The kernel space is statically defined beause it is needed
 * before the virtual memory has been setup or the kernel
 * memory allocator.
 */
ALIGNED(ARM_HWL1_SIZE) char UNIT("kspace") _kernel_space_pagetable[ARM_HWL1_SIZE];

void SECTION(".init") init_kernel_space()
{
    space_t * kspace = get_kernel_space();

    kspace->init_kernel_mappings();

    get_arm_fass()->init();

    kspace->enqueue_spaces();

}

/*
 * Window part of Armv5 specific space_control call
 */
word_t space_t::space_control_window(word_t ctrl)
{
    space_t * dest;
    spaceid_t dest_sid;
    bool r = false;
    dest_sid.set_raw(get_current_tcb()->get_mr(0));
    dest = get_current_space()->lookup_space(dest_sid);
    if (dest == NULL)
    {
        return 1;
    }

    if (ctrl == L4_SPACE_RESOURCES_WINDOW_MANAGE) {
        r = this->add_shared_domain(dest, true);
    } else if (ctrl == L4_SPACE_RESOURCES_WINDOW_GRANT) {
        r = this->add_shared_domain(dest, false);
    } else if (ctrl == L4_SPACE_RESOURCES_WINDOW_REVOKE) {
        r = this->remove_shared_domain(dest);
    } else {
        return 1;
    }
    return !r;
}

/**
 * initialize THE kernel space
 * @see get_kernel_space()
 */
void SECTION(".init") generic_space_t::init_kernel_mappings()
{
    /* Kernel space's mappings already setup in initial root page table */
}

/**
 * initialize a space
 *
 * @param utcb_page     fpage describing location of UTCB area
 */
bool generic_space_t::init (fpage_t utcb_page,
                            kmem_resource_t *kresource)
{
    ((space_t *)this)->set_domain(INVALID_DOMAIN);

    /* initialize UTCB pgentry in space */
    ((space_t *)this)->pgent_utcb()->clear(this, pgent_t::size_1m, true);

    /* Ignore user provided kip/user area */
    this->utcb_area.set(UTCB_AREA_START + get_space_id().get_spaceno()*ARM_SECTION_SIZE, BITS_WORD - ARM_SECTION_BITS, true, true, true);
    bitmap_init(((space_t *)this)->get_utcb_bitmap(), UTCB_BITMAP_LENGTH, true);
    bitmap_init(((space_t *)this)->get_client_spaces_bitmap(), CONFIG_MAX_SPACES, 0);
    bitmap_init(((space_t *)this)->get_shared_spaces_bitmap(), CONFIG_MAX_SPACES, 0);

    word_t section = ( UTCB_AREA_START/ARM_SECTION_SIZE +
            (get_space_id().get_spaceno()) );
    /* We have invalid domain, so store the space_t pointer in the
     * section */
    pgent_t entry;
    entry.raw = (word_t)this;
    get_cpd()[section] = entry;

    return true;
}

/**
 * Clean up a Space
 */
void generic_space_t::arch_free(kmem_resource_t *kresource)
{
    word_t section;

    section = ( UTCB_AREA_START/ARM_SECTION_SIZE + (get_space_id().get_spaceno()) );

    pgent_t entry, *pg;
    entry.raw = 0;
    /* We need to clear the space_t pointer in the cpd section */
    get_cpd()[section] = entry;

    pg = ((space_t *)this)->pgent_utcb();
    if (pg->raw) {
        pg->remove_subtree (this, pgent_t::size_1m, true, kresource);
    }

    space_t * spc = (space_t*)this;

    /* Clean the domain if needed */
    if (spc->get_domain() != INVALID_DOMAIN)
        get_arm_fass()->free_domain(spc);

    spc->flush_sharing_on_delete();
}


/**
 * Allocate a UTCB
 * @param tcb   Owner of the utcb
 */
utcb_t * generic_space_t::allocate_utcb(tcb_t * tcb,
                                        kmem_resource_t *kresource)
{
    bitmap_t * utcb_bitmap = ((space_t *)this)->get_utcb_bitmap();
    int pos = bitmap_findfirstset(utcb_bitmap, UTCB_BITMAP_LENGTH);

    if (pos == -1)
    {
        get_current_tcb()->set_error_code (EUTCB_AREA);
        return (utcb_t *)0;
    }

    bitmap_clear(utcb_bitmap, pos);

    utcb_t * utcb = (utcb_t*)( UTCB_AREA_START +
                    (get_space_id().get_spaceno() * ARM_SECTION_SIZE) +
                    (pos * UTCB_SIZE) );

    /* Try lookup the UTCB page for this utcb */
    space_t * space = (space_t *)this;
    pgent_t level1 = *space->pgent_utcb();

    bool is_valid = true;

    if (EXPECT_TRUE(level1.is_valid(space, pgent_t::size_1m)))
    {
        if (EXPECT_TRUE(level1.is_subtree(space, pgent_t::size_1m))) {
            pgent_t leaf =
                    *level1.subtree(space, pgent_t::size_1m)->next(
                                        space, UTCB_AREA_PGSIZE,
                                        ((word_t)utcb & (PAGE_SIZE_1M-1)) >> ARM_PAGE_BITS
                                        );
            if (leaf.l2.fault.zero == 0)
                is_valid = false;
        } else {
            //WARNING("1MB page in UTCB area");
            return (utcb_t *)0;
        }
    } else {
        is_valid = false;
    }

    /* No UTCB page present for this utcb, alloc and map a new one */
    if (!is_valid) {
        addr_t page = kresource->alloc (kmem_group_utcb, UTCB_AREA_PAGESIZE, true);
        if (page == NULL) {
            get_current_tcb()->set_error_code(ENO_MEM);
            return NULL;
        }
        arm_cache::cache_flush_d_ent(page, UTCB_AREA_PAGEBITS);

        if (space->add_mapping((addr_t)addr_align(utcb, UTCB_AREA_PAGESIZE),
                               virt_to_ram(page), UTCB_AREA_PGSIZE,
                               space_t::read_write, false, kresource) == false)
        {
            kresource->free(kmem_group_utcb, page, UTCB_AREA_PAGESIZE);
            get_current_tcb()->set_error_code(ENO_MEM);
            return NULL;
        }
    } else {
        /* Clear UTCB if page already present.  */
        word_t *source = (word_t*)utcb;
        word_t *source_end = source + (UTCB_SIZE / sizeof(word_t));

        while(source < source_end) {
            *source++ = 0x0;
        }
    }
    tcb->set_utcb_location((word_t)utcb);
    return utcb;
}

/**
 * Free a UTCB
 * @param utcb   The utcb
 */
void generic_space_t::free_utcb(utcb_t * utcb)
{
    /*
     * Sometimes thread creation fails due to out of memory,
     * this can cause utcb == NULL
     */
    if (utcb == NULL) {
        return;
    }
    /* We have to free the thread's UTCB */
    space_t *space = (space_t *)this;

    word_t offset, utcb_num;

    offset = (word_t)utcb - UTCB_AREA_START -
            (space->get_space_id().get_spaceno() * ARM_SECTION_SIZE);
    utcb_num = offset >> UTCB_BITS;

    /* Free this utcb */
    bitmap_set(space->get_utcb_bitmap(), utcb_num);

    /* Search to see if all UTCBs in this page are now free */
    offset = (offset & (~(UTCB_AREA_PAGESIZE-1)));
    utcb_num = offset >> UTCB_BITS;

    if (EXPECT_TRUE(!bitmap_israngeset(space->get_utcb_bitmap(), utcb_num,
                utcb_num + (UTCB_AREA_PAGESIZE>>UTCB_BITS) - 1)))
    {
        return;
    }

    pgent_t *pg;
    space_t *mapspace = space;

    pg = mapspace->pgent_utcb();
    if (pg->is_valid(mapspace, pgent_t::size_1m) &&
        pg->is_subtree(mapspace, pgent_t::size_1m))
    {
        pgent_t *subtree = pg->subtree(mapspace, pgent_t::size_1m);
        word_t utcb_section = ((word_t)utcb &(PAGE_SIZE_1M-1)) >> ARM_PAGE_BITS;

        pg = subtree->next(mapspace, UTCB_AREA_PGSIZE, utcb_section);
    } else {
        pg = NULL;
    }

    if (!pg || !pg->is_valid(mapspace, pgent_t::size_4k) ) {
        //enter_kdebug("UTCB mapping not found");
    }

    addr_t page = addr_align(utcb, UTCB_AREA_PAGESIZE);

    /* pass in page aligned address! */
    mapspace->free_utcb_page(pg, UTCB_AREA_PGSIZE, page);
}


void generic_space_t::flush_tlb(space_t *curspace)
{
    if (((space_t *)this)->get_domain() != INVALID_DOMAIN)
    {
        domain_dirty = current_domain_mask;
        arm_cache::cache_flush();
        arm_cache::tlb_flush();
    }
}

void generic_space_t::flush_tlbent_local(space_t *curspace, addr_t vaddr, word_t log2size)
{
    vaddr = addr_align(vaddr, 1 << log2size);
    arm_cache::cache_flush_ent(vaddr, log2size);
    arm_cache::tlb_flush_ent(vaddr, log2size);
}

bool generic_space_t::allocate_page_directory(kmem_resource_t *kresource)
{
#if (ARM_L0_SIZE < KMEM_CHUNKSIZE)
    pdir = (pgent_t *)kresource->alloc(kmem_group_l0_allocator, true);
#else
    pdir = (pgent_t *)kresource->alloc(kmem_group_pgtab, ARM_L0_SIZE, true);
#endif
    if (!pdir)
        return false;

    return true;
}

void generic_space_t::free_page_directory(kmem_resource_t *kresource)
{
#if (ARM_L0_SIZE < KMEM_CHUNKSIZE)
    kresource->free(kmem_group_l0_allocator, pdir);
#else
    kresource->free(kmem_group_pgtab, pdir, ARM_L0_SIZE);
#endif
}

/**
 * Set up hardware context to run the tcb in this space.
 */
void generic_space_t::activate(tcb_t *tcb)
{
    if (this != get_kernel_space())
    {
        USER_UTCB_REF = tcb->get_utcb_location();

        get_arm_globals()->current_clist = this->get_clist();
        get_arm_fass()->activate_domain((space_t *)this);
        /* Set PID of new space */
        write_cp15_register(C15_pid, C15_CRm_default, C15_OP2_default,
                            ((space_t *)this)->get_pid() << 25);
    }
    else
    {
        get_arm_fass()->activate_domain((space_t *)NULL);
    }
}

/*
 *  Domain Sharing
 */

/**
 * Is this space sharing the domain of target?
 */
bool space_t::is_sharing_domain(space_t *target)
{
    word_t targetid = target->get_space_id().get_spaceno();

    return bitmap_isset(this->get_shared_spaces_bitmap(), targetid);
}

/**
 * Is this space a manager of the domain of target?
 */
bool space_t::is_manager_of_domain(space_t *target)
{
    word_t targetid = target->get_space_id().get_spaceno();

    return bitmap_isset(this->get_manager_spaces_bitmap(), targetid);
}

bool space_t::add_shared_domain(space_t *space, bool manager)
{
    arm_domain_t domain = space->get_domain();

    //printf("insert shared  %p->%p : %d\n", this, space, domain);
    word_t clientid = space->get_space_id().get_spaceno();
    word_t thisid = this->get_space_id().get_spaceno();

    if (bitmap_isset(space->get_client_spaces_bitmap(), thisid)) {
    }

    bitmap_set(space->get_client_spaces_bitmap(), thisid);
    bitmap_set(this->get_shared_spaces_bitmap(), clientid);
    if (manager) {
        bitmap_set(this->get_manager_spaces_bitmap(), clientid);
    } else {
        bitmap_clear(this->get_manager_spaces_bitmap(), clientid);
    }

    /* update domain masks */
    if (domain != INVALID_DOMAIN) {
        this->add_domain_access(domain, manager);
        if (space == get_current_space()) {
            current_domain_mask = space->get_domain_mask();
            domain_dirty |= current_domain_mask;
        }
    }
    return true;
}

bool space_t::remove_shared_domain(space_t *space)
{
    word_t clientid = space->get_space_id().get_spaceno();
    word_t thisid = this->get_space_id().get_spaceno();

    if (!bitmap_isset(space->get_client_spaces_bitmap(), thisid)) {
        get_current_tcb()->set_error_code(EINVALID_PARAM);
        return false;
    }

    /* unlink this space from the shared space */
    (void) this->domain_unlink(space);

    arm_domain_t domain = space->get_domain();

    /* update domain masks */
    if (domain != INVALID_DOMAIN) {
        remove_domain_access(domain);
        if (space == get_current_space()) {
            current_domain_mask = space->get_domain_mask();
        }
    }

    bitmap_clear(space->get_client_spaces_bitmap(), thisid);
    bitmap_clear(this->get_shared_spaces_bitmap(), clientid);

    return true;
}

/*
 * Remove access to this space's domain,
 * base space must have valid domain!
 */
void space_t::flush_sharing_spaces(void)
{
    word_t i;

    if (bitmap_isallclear(this->get_client_spaces_bitmap(), CONFIG_MAX_SPACES)) {
        return;
    }

    for (i = 0; i < CONFIG_MAX_SPACES; i++) {
        if (bitmap_isset(this->get_client_spaces_bitmap(), i)) {
            space_t *space = get_space_list()->lookup_space(spaceid(i));

            /* Check for valid space id */
            if (EXPECT_FALSE( space == NULL ))
            {
            }
            space->remove_domain_access(this->domain);
            if (space == get_current_space()) {
                current_domain_mask = space->get_domain_mask();
            }
        }
    }
}

void space_t::flush_sharing_on_delete(void)
{
    word_t i;

    if (bitmap_isallclear(this->get_shared_spaces_bitmap(), CONFIG_MAX_SPACES)) {
    } else {
        for (i = 0; i < CONFIG_MAX_SPACES; i++) {
            if (bitmap_isset(this->get_shared_spaces_bitmap(), i)) {
                space_t *source = get_space_list()->lookup_space(spaceid(i));

                /* Check for valid space id */
                if (EXPECT_FALSE( source == NULL ))
                {
                    //panic("found null space in source list");
                }

                bitmap_clear(source->get_client_spaces_bitmap(), this->get_space_id().get_spaceno());
            }
        }
    }
    if (bitmap_isallclear(this->get_client_spaces_bitmap(), CONFIG_MAX_SPACES)) {
    } else {
        for (i = 0; i < CONFIG_MAX_SPACES; i++) {
            if (bitmap_isset(this->get_client_spaces_bitmap(), i)) {
                space_t *space = get_space_list()->lookup_space(spaceid(i));

                /* Check for valid space id */
                if (EXPECT_FALSE( space == NULL ))
                {
                }
                (void) space->remove_shared_domain(this);
            }
        }
    }
}


bool space_t::is_client_space(space_t *space)
{
    word_t spc_no = space->get_space_id().get_spaceno();

    if (bitmap_isset(this->get_client_spaces_bitmap(), spc_no)) {
        return true;
    }
    return false;
}

void space_t::set_vspace(word_t vspace)
{
    if ((vspace == 0) || get_current_space()->space_range.is_valid(vspace)) {
        this->bits.vspace = vspace;
    } else {
        this->bits.vspace = 0;
    }
}

