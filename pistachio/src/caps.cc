/*
 * Description: Capability management
 */

#include <l4.h>
#include <debug.h>
#include <tcb.h>
#include <caps.h>
#include <clist.h>
#include <kmemory.h>

SYS_CAP_CONTROL(clistid_t clist_id, cap_control_t control)
{
    LOCK_PRIVILEGED_SYSCALL();
    continuation_t continuation = ASM_CONTINUATION;
    kmem_resource_t *kresource;

    tcb_t *current = get_current_tcb();
    tcb_t *tcb_locked;
    clist_t * clist;

    clist = get_current_space()->lookup_clist(clist_id);

    switch (control.get_op())
    {
    case cap_control_t::op_create_clist:
        {
            word_t entries, alloc_size;

            /* Check that provided clist-id is valid */
            if (EXPECT_FALSE(!get_current_space()->clist_range.is_valid(
                             clist_id.get_clistno())))
            {
                current->set_error_code (EINVALID_CLIST);
                goto error_out;
            }

            /* Check for non-existing clist. */
            if (EXPECT_FALSE(clist != NULL))
            {
                current->set_error_code(EINVALID_CLIST);
                goto error_out;
            }

            kresource = get_current_kmem_resource();

            entries = current->get_mr(0);
            alloc_size = (word_t)addr_align_up((addr_t)(sizeof(cap_t) * entries + sizeof(clist_t)), KMEM_CHUNKSIZE);

            clist_t* new_list = (clist_t*) kresource->alloc(kmem_group_clist, alloc_size, true);
            if (new_list == NULL)
            {
                current->set_error_code(ENO_MEM);
                goto error_out;
            }

            new_list->init(entries);

            get_clist_list()->add_clist(clist_id, new_list);
            //printf("Create clist id: %ld size: %ld\n", clist_id.get_raw(), entries);
        }
        break;
    case cap_control_t::op_delete_clist:
        {
            word_t entries, alloc_size;

            /* Check that a valid clist was provided. */
            if (EXPECT_FALSE(clist == NULL))
            {
                current->set_error_code(EINVALID_CLIST);
                goto error_out;
            }

            /* Check for empty clist. */
            if (EXPECT_FALSE(clist->get_space_count() != 0))
            {
                current->set_error_code(ECLIST_NOT_EMPTY);
                goto error_out;
            }

            if (EXPECT_FALSE(!clist->is_empty()))
            {
                current->set_error_code(ECLIST_NOT_EMPTY);
                goto error_out;
            }

            entries = clist->num_entries();

            get_clist_list()->remove_clist(clist_id);

            kresource = get_current_kmem_resource();

            alloc_size = (word_t)addr_align_up((addr_t)(sizeof(cap_t) * entries + sizeof(clist_t)), KMEM_CHUNKSIZE);
            kresource->free(kmem_group_clist, clist, alloc_size);
            //printf("Deleted clist: %ld\n", clist_id.get_raw());
        }
        break;
    case cap_control_t::op_copy_cap:
        {
            capid_t src_id = capid(current->get_mr(0));
            clistid_t target_list_id = clistid(current->get_mr(1));
            capid_t target_id = capid(current->get_mr(2));
            clist_t *target_list;

            /* Check that a valid clist was provided. */
            if (EXPECT_FALSE(clist == NULL))
            {
                current->set_error_code(EINVALID_CLIST);
                goto error_out;
            }

            target_list = get_current_space()->lookup_clist(target_list_id);

            /* Check that a valid target clist was provided. */
            if (EXPECT_FALSE(target_list == NULL)) {
                current->set_error_code(EINVALID_CLIST);
                goto error_out;
            }

            /* Ensure target_id is within range */
            if (EXPECT_FALSE(!target_list->is_valid(target_id))) {
                current->set_error_code(EINVALID_CAP);
                goto error_out;
            }

            tcb_locked = clist->lookup_thread_cap_locked(src_id);
            if (EXPECT_FALSE(tcb_locked == NULL)) {
                current->set_error_code(EINVALID_CAP);
                goto error_out;
            }

            /* try to insert the cap, will fail if slot is used */
            if (!target_list->add_ipc_cap(target_id, tcb_locked)) {
                current->set_error_code(EINVALID_CAP);
                goto error_out_locked;
            }
            //printf("Add IPC cap: %lx to list %ld   - (copy from %lx  in list %ld)\n", target_id.get_raw(), target_list_id.get_raw(), src_id.get_raw(), clist_id.get_raw());
            tcb_locked->unlock_read();
        }
        break;
    case cap_control_t::op_delete_cap:
        {
            capid_t target_id = capid(current->get_mr(0));

            /* Check for valid clist id. */
            if (EXPECT_FALSE(clist == NULL))
            {
                current->set_error_code(EINVALID_CLIST);
                goto error_out;
            }

            /* Ensure target_id is within range */
            if (EXPECT_FALSE(!clist->is_valid(target_id))) {
                current->set_error_code(EINVALID_CAP);
                goto error_out;
            }

            /* try to remove the cap, will fail if not ipc-cap or invalid */
            if (!clist->remove_ipc_cap(target_id)) {
                current->set_error_code(EINVALID_CAP);
                goto error_out;
            }

            //printf("Del IPC cap: %lx in list %ld\n", target_id.get_raw(), clist_id.get_raw());
        }
        break;
    default:
        current->set_error_code(EINVALID_PARAM);
        goto error_out;
    }
    UNLOCK_PRIVILEGED_SYSCALL();
    return_cap_control(1, continuation);

error_out_locked:
    tcb_locked->unlock_read();
error_out:
    UNLOCK_PRIVILEGED_SYSCALL();
    return_cap_control(0, continuation);
}

