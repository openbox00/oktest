/*
 * Description:   Generic page table manipluation for ARMv5
 */
#ifndef __ARCH__ARM__V5__PGENT_H__
#define __ARCH__ARM__V5__PGENT_H__

#include <arch/fass.h>
#include <arch/ver/ptab.h>
#include <smallalloc.h>
#include <kmem_resource.h>


#define PGTABLE_OFFSET      (VIRT_ADDR_PGTABLE - get_arm_globals()->phys_addr_ram)

class generic_space_t;

template<typename T> INLINE T phys_to_page_table(T x)
{
    return (T) ((u32_t) x + PGTABLE_OFFSET);
}

/* before VM is initialised we cannot use the 
 * virtual addresses involved in PGTABLE offset
 * so use a pointer to the RAM address and do it
 * the hard way
 */ 
template<typename T> INLINE T phys_to_page_table_init(T x, word_t * p)
{
    return (T) ((u32_t) x + (VIRT_ADDR_PGTABLE - *p));
}

template<typename T> INLINE T page_table_to_phys(T x)
{
    return (T) ((u32_t) x - PGTABLE_OFFSET);
}

INLINE bool has_tiny_pages (generic_space_t * space)
{
    return false;
}

INLINE word_t arm_l2_ptab_size (generic_space_t * space)
{
    return has_tiny_pages (space) ? ARM_L2_SIZE_TINY : ARM_L2_SIZE_NORMAL;
}


class pgent_t
{
public:
    union {
        pgent_t *       tree;
        arm_l1_desc_t   l1;
        arm_l2_desc_t   l2;
        u32_t           raw;
    };

public:
    enum pgsize_e {
        size_4k     = 0,
        size_min    = size_4k,
        size_64k    = 1,
        size_1m     = 2,
        min_tree    = size_1m,
        size_level0 = 3,
        size_4g     = 4,
        size_max    = size_level0,
    };


private:


    void sync_64k (generic_space_t * s)
        {
            for (word_t i = 1; i < (arm_l2_ptab_size (s) >> 6); i++)
                ((u32_t *) this)[i] = raw;
        }

    void sync_64k (generic_space_t * s, word_t val)
        {
            for (word_t i = 1; i < (arm_l2_ptab_size (s) >> 6); i++)
                ((u32_t *) this)[i] = val;
        }

    void sync_4k (generic_space_t * s)
        {
            if (has_tiny_pages (s))
                for (word_t i = 1; i < 4; i++)
                    ((u32_t *) this)[i] = raw;
        }

public:
    bool is_valid (generic_space_t * s, pgsize_e pgsize)
        {
            switch (pgsize)
            {
            case size_level0:
                return (tree != NULL);
            case size_1m:
                return (l1.fault.zero != 0);
            case size_64k:
                return true;
            case size_4k:
                if (has_tiny_pages(s)) {
                    return true;
                } else {
                    return (l2.small.two == 2);
                }
            default:
                return false;
            }
        }
    word_t is_window (generic_space_t * s, pgsize_e pgsize)
        {
            switch (pgsize)
            {
            case size_level0:
                return 2;
            case size_1m:
                return ((l1.fault.zero == 0) && (l1.raw != 0)) ? 1 : 0;
            default:
                return 0;
            }
        }

    word_t is_window_level (generic_space_t * s, pgsize_e pgsize)
        {
            switch (pgsize)
            {
            case size_level0:
                return 2;
            case size_1m:
                return 1;
            default:
                return 0;
            }
        }

    bool is_writable (generic_space_t * s, pgsize_e pgsize)
        {
            return pgsize == size_1m ?
                    l1.section.ap == (word_t)arm_l1_desc_t::rw :
                    /* 64k, 4k, 1k have ap0 in the same location */
                    l2.tiny.ap == (word_t)arm_l1_desc_t::rw;
        }
    bool is_readable (generic_space_t * s, pgsize_e pgsize)
        { return l1.section.ap >= (word_t)arm_l1_desc_t::ro; }

    bool is_executable (generic_space_t * s, pgsize_e pgsize)
        { return l1.section.ap >= (word_t)arm_l1_desc_t::ro; }

    bool is_subtree (generic_space_t * s, pgsize_e pgsize)
        {
            switch (pgsize)
            {
            case size_level0:
                return (tree != NULL);
            case size_1m:
                return (l1.section.two != 2) && (l1.fault.zero != 0);
            case size_64k:
                return (l2.large.one != 1);
            case size_4k:
                if (has_tiny_pages(s)) {
                    return (l2.small.two != 2);
                }
                /* fall through */
            default:
                return false;
            }
        }

    bool is_kernel (generic_space_t * s, pgsize_e pgsize)
        { return l1.section.ap <= (word_t)arm_l1_desc_t::none; }
    arm_domain_t get_domain(void)
        {
            return l1.section.domain;
        }
    addr_t address (generic_space_t * s, pgsize_e pgsize)
        {
            return (addr_t)(raw & ~(0xfff));
        }

    pgent_t * subtree (generic_space_t * s, pgsize_e pgsize)
        {
            if (pgsize == size_level0)
            {
                return tree;
            }
            else if (pgsize == size_1m)
            {
                if (has_tiny_pages (s))
                    return (pgent_t *) phys_to_page_table(l1.address_finetable());
                else
                    return (pgent_t *) phys_to_page_table(l1.address_coarsetable());
            }
            else
                return this;
        }

    word_t reference_bits (generic_space_t * s, pgsize_e pgsize, addr_t vaddr)
        { return 7; }
public:
    void set_domain(arm_domain_t domain)
        {
        }
    void clear (generic_space_t * s, pgsize_e pgsize, bool kernel, addr_t vaddr)
        {
        }
    void clear (generic_space_t * s, pgsize_e pgsize, bool kernel )
        {
        }
    bool make_subtree (generic_space_t * s, pgsize_e pgsize, bool kernel,
                       kmem_resource_t *current)
        {
            if (pgsize == size_level0)
            {
                addr_t base = current->alloc (kmem_group_pgtab, ARM_L1_SIZE, true);
                if (base == NULL)
                    return false;
                tree = (pgent_t*)base;
            }
            else if (pgsize == size_1m)
            {
                if (has_tiny_pages (s))
                {
                    addr_t base = current->alloc (kmem_group_pgtab, ARM_L2_SIZE_TINY, true);
                    if (base == NULL)
                        return false;
                    l1.raw = 0;
                    l1.fine_table.three = 3;
                    l1.fine_table.base_address = (word_t)virt_to_ram(base) >> 12;
                }
                else
                {
                    addr_t base = current->alloc (kmem_group_pgtab, ARM_L2_SIZE_NORMAL, true);
                    if (base == NULL)
                        return false;
                    l1.raw = 0;
                    l1.coarse_table.one = 1;
                    l1.coarse_table.base_address = (word_t)virt_to_ram(base) >> 10;
                }
            }
            return true;
        }

    void remove_subtree (generic_space_t * s, pgsize_e pgsize, bool kernel,
                         kmem_resource_t *current)
        {
        }

    void set_window_faulting(generic_space_t * s, generic_space_t * target)
        {
        }

    void set_window_callback(generic_space_t * s, generic_space_t * target)
        {
        }

    void clear_window(generic_space_t * s)
        {
        }

    void * get_window(generic_space_t * s)
        {
            return (void*)(l1.raw | 0x80000000);
        }

    bool is_callback(void)
        {
            return (l1.raw & 0x80000000) == 0;
        }

#define QUAD(x) (x | (x<<2) | (x<<4) | (x<<6))

    void set_entry (generic_space_t * s, pgsize_e pgsize,
                           addr_t paddr, bool readable,
                           bool writable, bool executable,
                           bool kernel, memattrib_e attrib)
        {
            word_t ap;
            arm_l1_desc_t base;

            if (EXPECT_FALSE(kernel))
                ap = writable ? QUAD((word_t)arm_l1_desc_t::none) : QUAD((word_t)arm_l1_desc_t::special);
            else
                ap = writable ? QUAD((word_t)arm_l1_desc_t::rw) : QUAD((word_t)arm_l1_desc_t::ro);

            base.raw = 0;
            /* attrib bits in the same position for all page sizes */
            base.section.attrib = (word_t)attrib;

            if (EXPECT_TRUE(pgsize == size_4k))
            {
                arm_l2_desc_t l2_entry;

                l2_entry.raw = base.raw;
                l2_entry.small.two = 2;
                l2_entry.small.ap = ap;
                l2_entry.small.base_address = (word_t) paddr >> 12;

                l2.raw = l2_entry.raw;
            }
            else if (pgsize == size_1m)
            {
                arm_l1_desc_t l1_entry;

                l1_entry.raw = base.raw;
                l1_entry.section.two = 2;
                l1_entry.section.domain = 0;
                l1_entry.section.ap = ap;
                l1_entry.section.base_address = (word_t) paddr >> 20;

                l1.raw = l1_entry.raw;
            }
            else
            {   /* size_64k */
                arm_l2_desc_t l2_entry;

                l2_entry.raw = base.raw;
                l2_entry.large.one = 1;
                l2_entry.large.ap = ap;
                l2_entry.large.base_address = (word_t) paddr >> 16;

                l2.raw = l2_entry.raw;
                sync_64k (s);
            }
        }

    /* For init code, must be inline. Sets kernel and cached */
    inline void set_entry_1m (generic_space_t * s, addr_t paddr, bool readable,
                           bool writable, bool executable, memattrib_e attrib)
        {
            word_t ap;
            arm_l1_desc_t l1_entry;

            ap = writable ? (word_t)arm_l1_desc_t::none : (word_t)arm_l1_desc_t::special;

            l1_entry.raw = 0;
            l1_entry.section.two = 2;
            l1_entry.section.attrib = (word_t)attrib;
            l1_entry.section.domain = 0;
            l1_entry.section.ap = ap;
            l1_entry.section.base_address = (word_t) paddr >> 20;

            l1.raw = l1_entry.raw;
        }

    inline void set_entry (generic_space_t * s, pgsize_e pgsize,
                           addr_t paddr, bool readable,
                           bool writable, bool executable,
                           bool kernel)
        {
            set_entry(s, pgsize, paddr, readable, writable, executable, kernel, l4default);
        }

    memattrib_e get_attributes (generic_space_t * s, pgsize_e pgsize)
        {
            /* bits in the same position for all page sizes */
            return (memattrib_e)l2.small.attrib;
        }
    // Movement
    pgent_t * next (generic_space_t * s, pgsize_e pgsize, word_t num)
        {
            if (pgsize == size_64k)
            {
                return this + (num * (arm_l2_ptab_size (s) >> 6));
            }
            return this + num;
        }

};

#endif /* !__ARCH__ARM__V5__PGENT_H__ */
