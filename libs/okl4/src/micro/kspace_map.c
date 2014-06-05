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
_okl4_kspace_mapbyid(okl4_kspaceid_t kspaceid, okl4_kspace_map_attr_t *attr)
{
    okl4_word_t pages_mapped;
    okl4_word_t total_pages;

    total_pages = attr->range->size / attr->page_size;

    /* Perform the mappings. */
    for (pages_mapped = 0; pages_mapped < total_pages; pages_mapped++) {
        L4_MapItem_t map;
        okl4_word_t success;

        /* Setup the map item. */
        L4_MapItem_Map(&map, attr->target->segment_id,
                attr->target->range.base + pages_mapped * attr->page_size,
                attr->range->base + pages_mapped * attr->page_size,
                pagesize_to_bits(attr->page_size),
                attr->attributes, attr->perms);

        /* Perform the map. */
        success = L4_ProcessMapItem(kspaceid, map);
        if (!success) {
            return _okl4_convert_kernel_errno(L4_ErrorCode());
        }
    }

    return OKL4_OK;
}

int
okl4_kspace_map(okl4_kspace_t *kspace, okl4_kspace_map_attr_t *attr)
{
    return _okl4_kspace_mapbyid(kspace->id, attr);
}

