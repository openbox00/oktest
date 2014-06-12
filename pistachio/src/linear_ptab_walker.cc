/*
 * Description:   Linear page table manipulation
 */
#ifndef __GENERIC__LINEAR_PTAB_WALKER_CC__
#define __GENERIC__LINEAR_PTAB_WALKER_CC__

#include <l4.h>
#include <arch/pgent.h>
#include <fpage.h>
#include <tcb.h>
#include <space.h>
#include <schedule.h>

#include <linear_ptab.h>

const word_t hw_pgshifts[] = HW_PGSHIFTS;

class mapnode_t;

static inline addr_t address (fpage_t fp, word_t size) PURE;
static inline addr_t address (fpage_t fp, word_t size)
{
    return (addr_t) (fp.raw & ~((1UL << size) - 1));
}

static inline word_t dbg_pgsize (word_t sz)
{
    return (sz >= GB (1) ? sz >> 30 : sz >= MB (1) ? sz >> 20 : sz >> 10);
}

static inline char dbg_szname (word_t sz)
{
    return (sz >= GB (1) ? 'G' : sz >= MB (1) ? 'M' : 'K');
}

bool generic_space_t::map_fpage(phys_desc_t base, fpage_t dest_fp,
                                kmem_resource_t *kresource)
{
    word_t t_num = dest_fp.get_size_log2 ();
    pgent_t::pgsize_e pgsize, t_size;
    addr_t t_addr, p_addr;
    pgent_t * tpg;
    bool flush = false;

    pgent_t * r_tpg[pgent_t::size_max];
    word_t r_tnum[pgent_t::size_max];

    u64_t phys = base.get_base();
    if (EXPECT_FALSE(
                (phys & (((u64_t)1 << t_num)-1)) ||
                (t_num < page_shift(pgent_t::size_min))
                ))
    {
        return false;
    }

    t_addr = address (dest_fp, t_num);

    if (t_num > page_shift(pgent_t::size_max+1))
        t_num = page_shift(pgent_t::size_max+1);

    memattrib_e page_attributes = (memattrib_e)base.get_attributes();
    phys &= ~(((u64_t)1 << t_num)-1);
    p_addr = (addr_t)(word_t)phys;
    for (pgsize = pgent_t::size_max; page_shift(pgsize) > t_num; pgsize --) { }

    t_num = 1UL << (t_num - page_shift(pgsize));
    t_size =  pgent_t::size_max;

    tpg = this->pgent(0)->next(this, t_size, page_table_index(t_size, t_addr));

    begin_update ();
    while (t_num > 0)
    {
        if (! is_user_area (t_addr))
            break;

        if (EXPECT_FALSE(t_size > pgsize))
        {
            t_size --;
            r_tpg[t_size] = tpg->next (this, t_size+1, 1);
            r_tnum[t_size] = t_num - 1;
map_next_level:

            if (EXPECT_FALSE(! tpg->is_valid (this, t_size+1)))
            {
                if (!tpg->make_subtree (this, t_size+1, false, kresource)) {
                    goto map_fpage_fail;
                }
            }
            else if (EXPECT_FALSE(! tpg->is_subtree (this, t_size+1)))
            {
                t_size ++;
                if (
                        !this->is_mappable(t_addr, addr_offset (t_addr, page_size (t_size)))
                   )
                {
                    goto map_next_pgentry;
                }

                this->flush_tlbent_local (get_current_space (), t_addr, page_shift (t_size));
                tpg->clear(this, t_size, false, t_addr);
                continue;
            }
            tpg = tpg->subtree (this, t_size+1)->next
                (this, t_size, page_table_index (t_size, t_addr));
            continue;
        }
        else if (EXPECT_FALSE(tpg->is_valid (this, t_size) &&
                (tpg->is_subtree (this, t_size) ||
                 (tpg->address (this, t_size) != p_addr)) && this->is_mappable(t_addr, addr_offset (t_addr, page_size (t_size)))
                ))
        {
            word_t num = 1;
            pgent_t::pgsize_e s_size = t_size;
            addr_t vaddr = t_addr;

            while (num > 0)
            {
                if (! tpg->is_valid (this, t_size))
                {
                }
                else if (tpg->is_subtree (this, t_size))
                {
                    t_size--;

                    r_tpg[t_size] = tpg;
                    r_tnum[t_size] = num - 1;

                    tpg = tpg->subtree (this, t_size+1);
                    num = page_table_size (t_size);
                    continue;
                }
                else
                {
                    if (! this->does_tlbflush_pay (page_shift (t_size)))
                    {
                        this->flush_tlbent_local (get_current_space (), vaddr, page_shift (t_size));
                    }

                    tpg->clear(this, t_size, false, vaddr);
                }

                if (t_size < s_size)
                {
                    vaddr = addr_offset (vaddr, page_size (t_size));
                    tpg = tpg->next (this, t_size, 1);
                }

                num--;
                while (num == 0 && t_size < s_size)
                {
                    tpg = r_tpg[t_size];
                    num = r_tnum[t_size];
                    t_size++;
                    tpg->remove_subtree (this, t_size, false, kresource);
                    if (t_size < s_size)
                        tpg = tpg->next (this, t_size, 1);
                };

            }
            if (does_tlbflush_pay (page_shift (t_size)))
            {
                flush = true;
            }
        }
        else if (EXPECT_FALSE(tpg->is_valid (this, t_size) &&
                    tpg->is_subtree (this, t_size)))
        {
            goto map_recurse_down;
        }

        if (EXPECT_FALSE( !is_page_size_valid (t_size) ||
                          (((word_t)t_size > 0) &&
                           !this->is_mappable(t_addr, addr_offset (t_addr, page_size (t_size))) )))
        {
map_recurse_down:
            t_size--;
            r_tpg[t_size] = tpg->next (this, t_size+1, 1);
            r_tnum[t_size] = t_num - 1;

            t_num = page_table_size (t_size);
            goto map_next_level;
        }

        if (EXPECT_TRUE( this->is_mappable(t_addr) ))
        {
            bool valid = tpg->is_valid (this, t_size);
            if (EXPECT_FALSE(valid && !flush))
            {
                if (!does_tlbflush_pay (page_shift (t_size)))
                    flush_tlbent_local (get_current_space (), t_addr, page_shift (t_size));
                else
                    flush = true;
            }

            tpg->set_entry (this,
                    t_size, p_addr,
                    dest_fp.is_read(), dest_fp.is_write(),
                    dest_fp.is_execute(), false,
                    page_attributes);

            if (EXPECT_FALSE(valid && !flush))
            {
            }
        }

map_next_pgentry:
        t_addr = addr_offset (t_addr, page_size (t_size));
        p_addr = addr_offset (p_addr, page_size (t_size));
        t_num--;

        if (EXPECT_TRUE(t_num > 0))
        {
            tpg = tpg->next (this, t_size, 1);
            continue;
        }
        else if (t_size < pgsize)
        {
            do {
                tpg = r_tpg[t_size];
                t_num = r_tnum[t_size];
                t_size++;
            } while (t_size < pgsize && t_num == 0);
        }
    }

    if (flush)
        this->flush_tlb (get_current_space ());
    end_update ();
    return true;

map_fpage_fail:
    end_update ();
    return false;
}

void generic_space_t::read_fpage(fpage_t fpage, phys_desc_t *base, perm_desc_t *perm)
{
    word_t num = fpage.get_size_log2 ();
    addr_t vaddr = address (fpage, num);
    word_t rwx, RWX;
    pgent_t::pgsize_e pgsize;
    pgent_t * pg;
    bool exists;

    exists = true;

    base->clear();
    perm->clear();

    if (exists)
    {
        base->set_base((word_t)pg->address(this, pgsize));
        base->set_attributes((word_t)pg->get_attributes(this, pgsize));

        rwx = (pg->is_readable(this, pgsize) << 2) |
              (pg->is_writable(this, pgsize) << 1) |
              (pg->is_executable(this, pgsize) << 0);

        RWX = pg->reference_bits(this, pgsize, vaddr);

        perm->set_perms(rwx);
        perm->set_size(page_shift(pgsize));
        perm->set_reference(RWX);
    }
}

#endif /* !__GENERIC__LINEAR_PTAB_WALKER_CC__ */
