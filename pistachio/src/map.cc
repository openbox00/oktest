/*
 * Description:   Mapping implementation
 */

#include <l4.h>
#include <debug.h>
#include <kmemory.h>
#include <map.h>
#include <tcb.h>
#include <syscalls.h>

SYS_MAP_CONTROL (spaceid_t space_id, word_t control)
{
    LOCK_PRIVILEGED_SYSCALL();
    continuation_t continuation = ASM_CONTINUATION;
    NULL_CHECK(continuation);
    map_control_t ctrl;
    tcb_t *current;
    space_t *space, *curr_space;
    word_t count;
    kmem_resource_t *kresource;

    ctrl.set_raw(control);

    current = get_current_tcb();

    curr_space = get_current_space();
    space = curr_space->lookup_space(space_id);
    /*
     * Check for valid space id and that either the caller is the root task
     * or the destination space has the current space as its pager
     */
    if (EXPECT_FALSE(space == NULL))
    {
        current->set_error_code (EINVALID_SPACE);
        goto error_out;
    }

    if (EXPECT_FALSE(!is_kresourced_space(curr_space))) {
        current->set_error_code (EINVALID_SPACE);
        goto error_out;
    }

    if (EXPECT_FALSE(curr_space->phys_segment_list == NULL))
    {
        current->set_error_code (EINVALID_PARAM);
        goto error_out;
    }

    count = ctrl.highest_item()+1;

    /* check for message overflow !! */
    if (EXPECT_FALSE((count*3) > IPC_NUM_MR))
    {
        current->set_error_code (EINVALID_PARAM);
        goto error_out;
    }

    kresource = get_current_kmem_resource();

    for (word_t i = 0; i < count; i++)
    {
        phys_desc_t tmp_base;
        perm_desc_t tmp_perm;
        if (ctrl.is_window())
        {
            fpage_t fpg;
            space_t * target;
            spaceid_t target_sid;
            bool r;

            target_sid = spaceid(current->get_mr(i*2));
            fpg.raw = current->get_mr(i*2+1);

            target = get_current_space()->lookup_space(target_sid);
            // Check for valid space id
            if (EXPECT_FALSE (target == NULL))
            {
                get_current_tcb ()->set_error_code (EINVALID_SPACE);
                goto error_out;
            }

            if (space->is_sharing_domain(target)) {
                // printf("map: map window : %S -> %S - fpg: %lx\n", space, target, fpg.raw);
                r = space->window_share_fpage(target, fpg, kresource);

                if (r == false) {
                    /* Error code set in window_share_fpage() */
                    goto error_out;
                }
            } else {
                //printf("No permission - domain not shared!!\n");
                get_current_tcb ()->set_error_code (EINVALID_SPACE);
                goto error_out;
            }

            continue;
        }

        // Read the existing pagetable
        // Query will be removed soon.
        if (ctrl.is_query())
        {
            fpage_t fpg;
            virt_desc_t virt;
            segment_desc_t seg;

            tmp_perm.clear();

            virt.set_raw(current->get_mr((i * 3) + 2));
            seg.set_raw(current->get_mr(i * 3));

            fpg.raw = 0;
            fpg.x.base = virt.get_base() >> 10;

            space->read_fpage(fpg, &tmp_base, &tmp_perm);
        }

        // Modify the page table
        if (ctrl.is_modify())
        {
            phys_desc_t base;
            fpage_t fpg;
            virt_desc_t virt;
            segment_desc_t seg;
            size_desc_t size;
            phys_segment_t *segment;
            bool valid_bit_set;

            seg.set_raw(current->get_mr(i * 3));
            size.set_raw(current->get_mr((i * 3) + 1));
            virt.set_raw(current->get_mr((i * 3) + 2));

            if (size.get_valid() == 1) {
                valid_bit_set = true;
            } else {
                valid_bit_set = false;
            }

            /* We don't support multiple page batching yet */
            if (size.get_num_pages() != 1) {
                get_current_tcb()->set_error_code(EINVALID_PARAM);
                goto error_out;
            }

            fpg.raw = size.get_raw() & 0x7;
            fpg.x.size = size.get_page_size();
            fpg.x.base = virt.get_base() >> 10;

            base.set_attributes(((word_t)virt.get_attributes() & 0x3f));

            /*
             * Check for valid fpage
             */
            if (EXPECT_FALSE(fpg.get_address() != fpg.get_base()))
            {
                get_current_tcb ()->set_error_code (EINVALID_PARAM);
                goto error_out;
            }

            if (!valid_bit_set)
            {
                space->unmap_fpage(fpg, false, kresource);
            }
            else
            {
                /* We only check this if we are mapping. */
                segment = curr_space->phys_segment_list->lookup_segment(seg.get_segment());
                if (EXPECT_FALSE(!segment || !segment->is_contained(seg.get_offset(),
                                                        (1 << size.get_page_size())))) {
                    get_current_tcb()->set_error_code(EINVALID_PARAM);
                    goto error_out;
                }

                if (EXPECT_FALSE((segment->get_rwx() & size.get_rwx()) !=
                                                         size.get_rwx())) {
                    get_current_tcb()->set_error_code(EINVALID_PARAM);
                    goto error_out;
                }

                if (EXPECT_FALSE(!segment->attribs_allowed(virt.get_attributes()))) {
                    get_current_tcb()->set_error_code(EINVALID_PARAM);
                    goto error_out;
                }

                base.set_base(segment->get_base() + seg.get_offset());

                if (EXPECT_FALSE(!space->map_fpage(base, fpg, kresource)))
                {
                    /* Error code set in map_fpage */
                    goto error_out;
                }
            }
        }

        /*
         * Query will be removed from MapControl soon.
         */
        if (ctrl.is_query())
        {
            segment_desc_t seg;
            virt_desc_t virt;
            word_t query_desc;

            seg.set_offset(tmp_base.get_base());

            /* 
             * We don't have a query descriptor
             * because queries will be removed 
             */
            query_desc = 0UL;
            /* Setting the permissions and the page size */
            query_desc |= (tmp_perm.get_raw() & 0x3ff);
            /* Setting the references */
            query_desc |= tmp_perm.get_reference() << 10;

            virt.set_attributes((int)tmp_base.get_attributes());

            current->set_mr((i*3), seg.get_raw());
            current->set_mr((i*3)+1, query_desc);
            current->set_mr((i*3)+2, virt.get_raw());
        }

    }
    UNLOCK_PRIVILEGED_SYSCALL();
    return_map_control(1, continuation);

error_out:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_map_control(0, continuation);
}
