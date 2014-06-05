#include <compat/c.h>

#include <l4/config.h>
#include <l4/map.h>
#include <l4/space.h>

#include <okl4/kspace.h>

static okl4_word_t
pagesize_to_bits(okl4_word_t n)
{
    return 31 - __clz(n);
}


int
_okl4_kspace_unmapbyid(okl4_kspaceid_t kspaceid, okl4_kspace_unmap_attr_t *attr)
{
    okl4_word_t total_pages;
    okl4_word_t pages_mapped;

    total_pages = attr->range->size / attr->page_size;

    /* Perform the unmap. */
    for (pages_mapped = 0; pages_mapped < total_pages; pages_mapped++) {
        L4_MapItem_t map;

        /* Setup the map item. */
        L4_MapItem_Unmap(&map,
                attr->range->base + pages_mapped * attr->page_size,
                pagesize_to_bits(attr->page_size));

        /* Perform the map. We allow the map to fail, as the user
         * may wish to unmap a sparse area of the address space. */
        (void)L4_ProcessMapItem(kspaceid, map);
    }

    return OKL4_OK;
}

int
okl4_kspace_unmap(okl4_kspace_t *kspace, okl4_kspace_unmap_attr_t *attr)
{
    return _okl4_kspace_unmapbyid(kspace->id, attr);
}

