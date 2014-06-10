/*
 * Description:   Support functions for ARM platforms.
 */
#include <l4.h>
#include <soc/interface.h>
#include <schedule.h>
#include <debug.h>
#include <space.h>
#include <arch/pgent.h>
#include <arch/page.h>

#define MAX_IO_AREAS 16

bitmap_t mappings = 0;

#define ROUND_SIZE(sz, pgsz, fsz, failure) do {  \
    if (sz <= PAGE_SIZE_4K) {                 \
        pgsz = pgent_t::size_4k;              \
        fsz  = PAGE_SIZE_4K;                  \
    } else if (sz <= PAGE_SIZE_1M) {          \
        pgsz = pgent_t::size_1m;              \
        fsz = PAGE_SIZE_1M;                   \
    } else {                                  \
        fsz = failure;                        \
    }                                         \
} while(0)


extern "C" addr_t
kernel_add_mapping(word_t size,
                   addr_t phys,
                   word_t cache_attr)
{
    space_t           *space = get_kernel_space();
    addr_t             base;
    pgent_t::pgsize_e  pgsize = pgent_t::size_4k; // Avoiding flint warning
    word_t             final_size;
    word_t             i;

    /*
     * This is very simple (and wasteful of virtual addr space.
     */
    ROUND_SIZE(size, pgsize, final_size, 0);

    if (final_size == 0) {
        return NULL;
    }

    for (i = 0 ; i < MAX_IO_AREAS ; i++) {
        if (!bitmap_isset(&mappings, i))
            {
                break;
            }
    }

    if (i == MAX_IO_AREAS) {
        return NULL; // Out of spaces
    }

    base = (addr_t)(IO_AREA_START + ARM_SECTION_SIZE * i);

    bitmap_set(&mappings, i);

    kmem_resource_t *kresource = get_kernel_space()->get_kmem_resource();

    if (space->add_mapping(base, phys, pgsize, space_t::read_write_ex, true,
                           (memattrib_e)cache_attr, kresource)) {
    } else {
        base = (addr_t)0;
    }
    return base;
}
