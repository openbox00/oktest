/*
 * Description:   architecture independent parts of space_t
 */

#include <tcb.h>
#include <linear_ptab.h>

space_t * global_spaces_list = NULL;

INLINE addr_t addr_align (addr_t addr, word_t align)
{
    return addr_mask (addr, ~(align - 1));
}


void SECTION(SEC_INIT) init_spaceids(word_t max_spaceids,
                                     kmem_resource_t *kresource)
{
    void * new_table;
    new_table = kresource->alloc(kmem_group_spaceids, sizeof(space_t*) * max_spaceids, true);
}

space_t * allocate_space(kmem_resource_t *res, spaceid_t space_id, clist_t *clist)
{

    space_t * space = (space_t *) res->alloc(kmem_group_space, true);
    if (EXPECT_FALSE(!space)) {
        return NULL;
    }
    if (!space->allocate_page_directory(res)) {
        res->free(kmem_group_space, space);
        return NULL;
    }

    space->set_kmem_resource(NULL);
    space->enqueue_spaces();
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
