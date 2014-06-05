#include <okl4/kspace.h>

int
okl4_kspace_create(okl4_kspace_t *kspace, okl4_kspace_attr_t *attr)
{
    okl4_word_t result;
#if !defined(NO_UTCB_RELOCATE)
    okl4_word_t utcb_base, utcb_size;
#endif
    L4_Fpage_t utcb_area;

    if (attr != OKL4_WEAVED_OBJECT) {
        kspace->id = attr->id;
        kspace->kclist = attr->kclist;
        kspace->utcb_area = attr->utcb_area;
        kspace->kthread_list = NULL;
        kspace->next = NULL;
        kspace->privileged = attr->privileged;
    }

    /*
     * Now create the actual kernel space using SpaceControl syscall
     * Create a Fpage for a utcb and then pass it along
     */
#if !defined(NO_UTCB_RELOCATE)
    utcb_base = kspace->utcb_area->utcb_memory.base;
    utcb_size = kspace->utcb_area->utcb_memory.size;


    utcb_area = L4_Fpage(utcb_base, utcb_size);
#else
    utcb_area = L4_Nilpage;
#endif

    result = L4_SpaceControl(kspace->id,
            L4_SpaceCtrl_new
                | (attr->privileged ? L4_SpaceCtrl_kresources_accessible : 0),
            kspace->kclist->id, utcb_area, 0, NULL);
    if (result != 1) {
        return _okl4_convert_kernel_errno(L4_ErrorCode());
    }

    /* Add this kspace into the kclist's list of kspaces */
    kspace->next = kspace->kclist->kspace_list;
    kspace->kclist->kspace_list = kspace;

    return OKL4_OK;
}
