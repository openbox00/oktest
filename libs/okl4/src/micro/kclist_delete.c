#include <okl4/kclist.h>
#include <l4/caps.h>

void
okl4_kclist_delete(okl4_kclist_t * kclist)
{
    okl4_word_t ret;
    ret = L4_DeleteClist(kclist->id);

    /* Destroy the bitmap allocator */
    okl4_bitmap_allocator_destroy(&kclist->kcap_allocator);
}
