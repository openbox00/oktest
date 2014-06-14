/*
 * Description:   Helper functions for generic linear page table
 */
#ifndef __LINEAR_PTAB_H__
#define __LINEAR_PTAB_H__

#include <arch/pgent.h>
#include <space.h>

extern const word_t hw_pgshifts[];

INLINE pgent_t::pgsize_e operator-- (pgent_t::pgsize_e & l, int)
{
    pgent_t::pgsize_e ret = l;
    l = (pgent_t::pgsize_e) ((word_t) l - 1);
    return ret;
}


INLINE pgent_t::pgsize_e operator++ (pgent_t::pgsize_e & l, int)
{
    pgent_t::pgsize_e ret = l;
    l = (pgent_t::pgsize_e) ((word_t) l + 1);
    return ret;
}

INLINE pgent_t::pgsize_e operator+ (pgent_t::pgsize_e l, int r)
{
    return (pgent_t::pgsize_e) ((word_t) l + r);
}

INLINE pgent_t::pgsize_e operator- (pgent_t::pgsize_e l, int r)
{
    return (pgent_t::pgsize_e) ((word_t) l - r);
}

word_t page_size (pgent_t::pgsize_e pgsize) PURE;

INLINE word_t page_size (pgent_t::pgsize_e pgsize)
{
    return 1UL << hw_pgshifts[pgsize];
}

word_t page_shift (pgent_t::pgsize_e pgsize) PURE;

INLINE word_t page_shift (pgent_t::pgsize_e pgsize)
{
    return hw_pgshifts[pgsize];
}

word_t page_mask (pgent_t::pgsize_e pgsize) PURE;

INLINE word_t page_mask (pgent_t::pgsize_e pgsize)
{
    return ((1UL << hw_pgshifts[pgsize]) - 1);
}

INLINE word_t page_table_size (pgent_t::pgsize_e pgsize)
{
    return 1UL << (hw_pgshifts[pgsize+1] - hw_pgshifts[pgsize]);
}

INLINE word_t page_table_index (pgent_t::pgsize_e pgsize, addr_t vaddr)
{
    return ((word_t) vaddr >> hw_pgshifts[pgsize]) &
        (page_table_size (pgsize) - 1);
}

bool is_page_size_valid (pgent_t::pgsize_e pgsize) PURE;

INLINE bool is_page_size_valid (pgent_t::pgsize_e pgsize)
{
    return (1UL << hw_pgshifts[pgsize]) & HW_VALID_PGSIZES;
}

template<typename T>
INLINE bool readmem (space_t * space, addr_t vaddr, T * v)
{
    word_t w;
    if (space == (void*)~0UL)
    {
        addr_t paddr = vaddr;
        addr_t paddr1 = addr_mask (paddr, ~(sizeof (word_t) - 1));

        if (paddr1 == paddr)
        {
            w = get_kernel_space()->readmem_phys (vaddr, paddr);
        }
        else
        {
            addr_t vaddr1 = addr_mask (vaddr, ~(sizeof (word_t) - 1));
            addr_t vaddr2 = addr_offset (vaddr1, sizeof (word_t));
            addr_t paddr2 = addr_offset (paddr1, sizeof (word_t));

            word_t idx = ((word_t) vaddr) & (sizeof (word_t) - 1);

#if defined(CONFIG_BIGENDIAN)
            w = (get_kernel_space()->readmem_phys (vaddr1, paddr1) << (idx * 8)) |
                (get_kernel_space()->readmem_phys (vaddr2, paddr2) >> ((sizeof (word_t) - idx) * 8));
#else
            w = (get_kernel_space()->readmem_phys (vaddr1, paddr1) >> (idx * 8)) |
                (get_kernel_space()->readmem_phys (vaddr2, paddr2) << ((sizeof (word_t) - idx) * 8));
#endif
        }
    }
    else
    {
        if (! space->is_user_area (vaddr))
        {
            *v = *(T *) vaddr;
            return true;
        }
        if (! space->readmem (vaddr, &w))
            return false;
    }

#if defined(CONFIG_BIGENDIAN)
    w >>= (sizeof (word_t) - sizeof (T)) * 8;
#endif

    switch (sizeof (T))
    {
    case 1: *v = (T) (w & 0xff); break;
    case 2: *v = (T) (w & 0xffff); break;
    case 4: *v = (T) (w & 0xffffffff); break;
    case 8: *v = (T) (w); break;
    }

    return true;
}


#endif /* !__LINEAR_PTAB_H__ */
