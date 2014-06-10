/*
 * Description:   Linear page table manipulation - ARM specifics
 */
#include <l4.h>
#include <linear_ptab.h>
#include <space.h>
#include <tcb.h>
#include <arch/pgent.h>

bool space_t::domain_unlink(generic_space_t *source)
{
    addr_t vaddr = (addr_t)0;

    word_t num;
    pgent_t *t_pgs[pgent_t::size_max] = { NULL };
    word_t r_num[pgent_t::size_max];

    pgent_t *t_pg = this->pgent(0);

    pgent_t::pgsize_e pgsize = pgent_t::size_max;
    num = page_table_size(pgsize);

    while (num)
    {
        if (!is_user_area(vaddr)) {
            break;
        }

        if (t_pg->is_valid(this, pgsize) &&
                (t_pg->is_window_level(this, pgsize) == 2))
        {

            // Recurse into subtree
            if ((word_t)pgsize == 0)
                return false;

            t_pgs[pgsize-1] = t_pg;
            r_num[pgsize-1] = num;

            t_pg = t_pg->subtree(this, pgsize);
            pgsize--;

            num = page_table_size(pgsize);
            continue;
        }

        if (t_pg->is_window(this, pgsize) == 1)
        {
            if (t_pg->get_window(this) == source)
            {
                t_pg->clear_window(this);
            }
        }

        num --;
        t_pg = t_pg->next(this, pgsize, 1);
        vaddr = addr_offset(vaddr, page_size(pgsize));

        while ((num == 0) && (pgsize < pgent_t::size_max)) {
            t_pg = t_pgs[pgsize];
            num = r_num[pgsize];
            pgsize++;

            num--;
            t_pg = t_pg->next(this, pgsize, 1);
        }
    }

    return true;
}

bool space_t::window_share_fpage(space_t *space, fpage_t fpage,
                                 kmem_resource_t *kresource)
{
    addr_t vaddr = (addr_t)0;

    word_t num;
    pgent_t *t_pgs[pgent_t::size_max] = { NULL };
    word_t r_num[pgent_t::size_max];

    pgent_t *t_pg = this->pgent(0);

    pgent_t::pgsize_e pgsize = pgent_t::size_max;
    num = page_table_size(pgsize);

    while (num)
    {
        if (!is_user_area(vaddr)) {
            break;
        }
        if (!fpage.is_range_overlapping(vaddr, addr_offset(vaddr, page_size(pgsize)))) {
            goto next;
        }
        if (!t_pg->is_valid(this, pgsize) &&
                (t_pg->is_window_level(this, pgsize) == 2))
        {
            if (!t_pg->make_subtree(this, pgsize, false, kresource)) {
            }
        }

        if (t_pg->is_valid(this, pgsize))
        {
            if (t_pg->is_subtree(this, pgsize))
            {
                if (t_pg->is_window_level(this, pgsize) < 2)
                {
                    get_current_tcb ()->set_error_code(EDOMAIN_CONFLICT);
                    goto error_out;
                }

                // Recurse into subtree
                if ((word_t)pgsize == 0)
                    return false;

                t_pgs[pgsize-1] = t_pg;
                r_num[pgsize-1] = num;

                t_pg = t_pg->subtree(this, pgsize);
                pgsize--;

                num = page_table_size(pgsize);
                continue;
            }
            else
            {
                get_current_tcb ()->set_error_code(EDOMAIN_CONFLICT);
                goto error_out;
            }
        }
        else
        {
            switch (t_pg->is_window(this, pgsize)) {
            case 1:
                if (fpage.get_rwx() == 0) {
                    t_pg->clear_window(this);
                } else {
                    get_current_tcb ()->set_error_code(EDOMAIN_CONFLICT);
                    goto error_out;
                }
                break;
            case 0:
                if (fpage.get_rwx()) {
                    if (fpage.is_meta() == 0) {
                        t_pg->set_window_faulting(this, space);
                    } else {
                        t_pg->set_window_callback(this, space);
                    }
                } else {
                }
                //printf(" -- : %p / %lx\n", t_pg, t_pg->l1.raw);
                break;
            case 2:
            default:
                //panic("bad addresspace state\n");
                break;
            }
        }

next:
        num --;
        t_pg = t_pg->next(this, pgsize, 1);
        vaddr = addr_offset(vaddr, page_size(pgsize));

        while ((num == 0) && (pgsize < pgent_t::size_max)) {
            t_pg = t_pgs[pgsize];
            num = r_num[pgsize];
            pgsize++;

            num--;
            t_pg = t_pg->next(this, pgsize, 1);
        }
    }

    return true;

error_out:
    return false;
}

