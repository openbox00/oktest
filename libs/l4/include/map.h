/*
 * Description:   Interfaces for mappings in map control
 */
#ifndef __L4__MAP_H__
#define __L4__MAP_H__

#include <l4/types.h>
#include <l4/map_types.h>
#include <l4/arch/config.h>

#define L4_MAX_MAP_ITEMS    (__L4_NUM_MRS / 3)

/*
 * MapItem and QueryItem
 */

typedef union {
    L4_Word_t raw;
    virtual_descriptor_t X;
} L4_VirtualDesc_t;

typedef union {
    L4_Word_t raw;
    size_descriptor_t X;
} L4_SizeDesc_t;

typedef union {
    L4_Word_t raw;
    segment_descriptor_t X;
} L4_SegmentDesc_t;

typedef struct {
    L4_SegmentDesc_t    seg;
    L4_SizeDesc_t       size;
    L4_VirtualDesc_t    virt;
} L4_MapItem_t;

L4_INLINE void
L4_MapItem_Map(L4_MapItem_t *m, L4_Word_t segment_id, L4_Word_t offset,
               L4_Word_t vaddr, L4_Word_t page_size, L4_Word_t attr,
               L4_Word_t rwx)
{
    L4_SegmentDesc_t    seg;
    L4_SizeDesc_t       size;
    L4_VirtualDesc_t    virt;

    seg.X.segment = segment_id;
    seg.X.offset = offset >> __L4_MAP_SHIFT;

    size.X.num_pages = 1;
    size.X.valid = 1;
    size.X.page_size = page_size;
    size.X.rwx = rwx;

    virt.raw = 0UL;
    virt.X.vbase = vaddr >> __L4_MAP_SHIFT;
    virt.X.attr = attr;

    m->seg = seg;
    m->size = size;
    m->virt = virt;
}

L4_INLINE void
L4_MapItem_MapReplace(L4_MapItem_t *m, L4_Word_t segment_id, L4_Word_t offset,
               L4_Word_t vaddr, L4_Word_t page_size, L4_Word_t attr,
               L4_Word_t rwx)
{
    L4_MapItem_Map(m, segment_id, offset, vaddr, page_size, attr, rwx);
    m->virt.X.replace = 1;
}

L4_INLINE void
L4_MapItem_Unmap(L4_MapItem_t *m, L4_Word_t vaddr, L4_Word_t page_size)
{
    L4_SegmentDesc_t    seg;
    L4_SizeDesc_t       size;
    L4_VirtualDesc_t    virt;

    seg.raw = 0;
    size.raw = 0;
    virt.raw = 0;

    size.X.valid = 0;
    size.X.num_pages = 1;
    size.X.page_size = page_size;
    virt.X.vbase = vaddr >> __L4_MAP_SHIFT;

    m->seg = seg;
    m->size = size;
    m->virt = virt;
}

L4_INLINE void
L4_MapItem_SparseUnmap(L4_MapItem_t *m, L4_Word_t vaddr, L4_Word_t page_size)
{
    L4_MapItem_Unmap(m, vaddr, page_size);
    m->virt.X.sparse = 1;
}

L4_INLINE void
L4_MapItem_SetMultiplePages(L4_MapItem_t *m, L4_Word_t num_pages)
{
    m->size.X.num_pages = num_pages;
}

L4_INLINE L4_Bool_t
L4_PageRightsSupported(L4_Word_t rwx)
{
    return ((1 << rwx) & __L4_VALID_HW_PAGE_PERMS);
}


#endif /* !__L4__MAP_H__ */
