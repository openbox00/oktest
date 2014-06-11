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
#include <kernel/arch/special.h>
#include <kernel/bitmap.h>
#include <linear_ptab.h>
#include <kernel/generic/lib.h>
#include <smallalloc.h>
#include <phys_segment.h>

space_t * global_spaces_list = NULL;
spinlock_t spaces_list_lock;

INLINE addr_t addr_align (addr_t addr, word_t align)
{
    return addr_mask (addr, ~(align - 1));
}


/* Table containing mappings from spaceid_t to space_t* */
spaceid_lookup_t space_lookup;

void SECTION(SEC_INIT) init_spaceids(word_t max_spaceids,
                                     kmem_resource_t *kresource)
{

    void * new_table;

    new_table = kresource->alloc(kmem_group_spaceids,
                                 sizeof(space_t*) * max_spaceids, true);

    space_lookup.init(new_table, max_spaceids);
}

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

    get_space_list()->add_space(space_id, space);

    space->init_security(clist);

    return space;
}

bool SECTION(SEC_INIT)
    map_region (space_t * space, word_t vaddr, word_t paddr, word_t size, word_t attr, word_t rwx, kmem_resource_t *kresource)
{
    phys_desc_t phys_desc;
    word_t supported_sizes = HW_VALID_PGSIZES;
    word_t pgsize, pagesize = 0;
    fpage_t fpg;

    size = (word_t)addr_align_up((addr_t)(((word_t)vaddr) + size), page_size(pgent_t::size_min)) - vaddr;
    vaddr = (word_t)addr_align((addr_t)vaddr, page_size(pgent_t::size_min));
    paddr = (word_t)addr_align((addr_t)paddr, page_size(pgent_t::size_min));

    while (size != 0)
    {
        /* find pagesize to use for this portion of the mapping */
        for (pgsize = BITS_WORD; pgsize; pgsize--) {
            pagesize = 1UL << pgsize;
            if (!(pagesize & supported_sizes)) {
                continue;
            }
            if (size < pagesize) {
                continue;
            }

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
        return get_clist_list()->lookup_clist(clist_id);
}

space_t * generic_space_t::lookup_space(spaceid_t space_id)
{
        return get_space_list()->lookup_space(space_id);
}

