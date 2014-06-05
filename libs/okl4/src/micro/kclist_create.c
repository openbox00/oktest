#include <okl4/kclist.h>
#include <l4/caps.h>

int
okl4_kclist_create(okl4_kclist_t * kclist,
        okl4_kclist_attr_t * attr)
{
    okl4_word_t result;

    /* If the attr == WEAVED_OBJECT, we assume that the object
     * is wevaed in and hence, initialization is not required
     */
    if (attr != OKL4_WEAVED_OBJECT) {
        kclist->id  = attr->id;
        kclist->num_cap_slots = attr->num_cap_slots;
        kclist->kspace_list = NULL;

        okl4_bitmap_allocator_init(&kclist->kcap_allocator,
                &attr->bitmap_attr);
    }

    result = L4_CreateClist(kclist->id, kclist->num_cap_slots);
    if (result != 1) {
        okl4_bitmap_allocator_destroy(&kclist->kcap_allocator);
        return _okl4_convert_kernel_errno(L4_ErrorCode());
    }

    return OKL4_OK;
}


