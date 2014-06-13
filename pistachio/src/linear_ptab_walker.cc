/*
 * Description:   Linear page table manipulation
 */
#ifndef __GENERIC__LINEAR_PTAB_WALKER_CC__
#define __GENERIC__LINEAR_PTAB_WALKER_CC__

#include <tcb.h>
#include <linear_ptab.h>

const word_t hw_pgshifts[] = HW_PGSHIFTS;

static inline addr_t address (fpage_t fp, word_t size) PURE;
static inline addr_t address (fpage_t fp, word_t size)
{
    return (addr_t) (fp.raw & ~((1UL << size) - 1));
}

bool generic_space_t::map_fpage(phys_desc_t base, fpage_t dest_fp,
                                kmem_resource_t *kresource)
{
    word_t t_num = dest_fp.get_size_log2 ();
    pgent_t::pgsize_e pgsize, t_size;
    addr_t t_addr, p_addr;
    pgent_t * tpg;

    pgent_t * r_tpg[pgent_t::size_max];
    word_t r_tnum[pgent_t::size_max];

    u64_t phys = base.get_base();
    t_addr = address (dest_fp, t_num);

    memattrib_e page_attributes = (memattrib_e)base.get_attributes();
    phys &= ~(((u64_t)1 << t_num)-1);
    p_addr = (addr_t)(word_t)phys;

    t_num = 1UL << (t_num - page_shift(pgsize));
    t_size =  pgent_t::size_max;

    tpg = this->pgent(0)->next(this, t_size, page_table_index(t_size, t_addr));

    while (t_num > 0)
    {
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
            tpg->set_entry (this,
                    t_size, p_addr,
                    dest_fp.is_read(), dest_fp.is_write(),
                    dest_fp.is_execute(), false,
                    page_attributes);
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
    return true;

map_fpage_fail:
    return false;
}
#endif /* !__GENERIC__LINEAR_PTAB_WALKER_CC__ */
