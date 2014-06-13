/*
 * Description: ARM space_t implementation.
 */
#include <linear_ptab.h>
#include <space.h>              /* space_t              */
#include <tcb.h>
#include <arch/pgent.h>
#include <config.h>
#include <arch/fass.h>
#include <kernel/arch/special.h>
#include <kernel/bitmap.h>
#include <kernel/generic/lib.h>
#include <kmem_resource.h>

INLINE addr_t addr_align (addr_t addr, word_t align)
{
    return addr_mask (addr, ~(align - 1));
}

ALIGNED(ARM_HWL1_SIZE) char UNIT("kspace") _kernel_space_pagetable[ARM_HWL1_SIZE];

void SECTION(".init") init_kernel_space()
{
    space_t * kspace = get_kernel_space();
    kspace->init_kernel_mappings();
    kspace->enqueue_spaces();
}

/*
 * Window part of Armv5 specific space_control call
 */
word_t space_t::space_control_window(word_t ctrl)
{
    return 1;
}

void SECTION(".init") generic_space_t::init_kernel_mappings()
{
}

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
            return (utcb_t *)0;
        }
    } else {
        is_valid = false;
    }

    /* No UTCB page present for this utcb, alloc and map a new one */
    if (!is_valid) {
        addr_t page = kresource->alloc (kmem_group_utcb, UTCB_AREA_PAGESIZE, true);
        if (page == NULL) {
            return NULL;
        }
        if (space->add_mapping((addr_t)addr_align(utcb, UTCB_AREA_PAGESIZE),
                               virt_to_ram(page), UTCB_AREA_PGSIZE,
                               space_t::read_write, false, kresource) == false)
        {
            kresource->free(kmem_group_utcb, page, UTCB_AREA_PAGESIZE);
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
}


void generic_space_t::flush_tlb(space_t *curspace)
{
}

void generic_space_t::flush_tlbent_local(space_t *curspace, addr_t vaddr, word_t log2size)
{
    vaddr = addr_align(vaddr, 1 << log2size);
}

bool generic_space_t::allocate_page_directory(kmem_resource_t *kresource)
{
    pdir = (pgent_t *)kresource->alloc(kmem_group_l0_allocator, true);
    if (!pdir)
        return false;

    return true;
}

void generic_space_t::free_page_directory(kmem_resource_t *kresource)
{
    kresource->free(kmem_group_l0_allocator, pdir);
}


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
        this->bits.vspace = 0;
}

