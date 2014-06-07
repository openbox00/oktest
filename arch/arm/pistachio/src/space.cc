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
#include <cpu/syscon.h>
#include <kernel/arch/special.h>
#include <kernel/bitmap.h>
#include <kernel/generic/lib.h>


extern bitmap_t ipc_bitmap_ids;

bool space_t::add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                          rwx_e rwx, bool kernel, memattrib_e attrib,
                          kmem_resource_t *kresource)
{
    pgent_t::pgsize_e pgsize = pgent_t::size_max;
    pgent_t *pg = this->pgent(0);
    pg = pg->next(this, pgsize, page_table_index(pgsize, vaddr));

    if (is_utcb_area(vaddr) && (this != get_kernel_space())) {
        /* Special case for UTCB mappings */
        pgsize = pgent_t::size_1m;
        pg = this->pgent_utcb();
    }
    /*
     * Lookup mapping
     */
    while (1) {
        if ( pgsize == size )
            break;

        // Create subtree
        if ( !pg->is_valid( this, pgsize ) ) {
            if (!pg->make_subtree( this, pgsize, kernel, kresource))
                return false;
        }

        pg = pg->subtree( this, pgsize )->next
            ( this, pgsize-1, page_table_index( pgsize-1, vaddr ) );
        pgsize--;
    }

    bool r = (word_t)rwx & 4;
    bool w = (word_t)rwx & 2;
    bool x = (word_t)rwx & 1;

    pg->set_entry(this, pgsize, paddr, r, w, x, kernel, attrib);
    arm_cache::cache_drain_write_buffer();

    return true;
}

