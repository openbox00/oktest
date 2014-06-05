#include <okl4/memsec.h>

int
okl4_memsec_premap(okl4_memsec_t *ms, okl4_word_t vaddr,
        okl4_physmem_item_t *phys, okl4_word_t *dest_vaddr,
        okl4_word_t *page_size, okl4_word_t *perms, okl4_word_t *attributes)
{
    /* Perform the premap call. */
    int error = ms->premap_callback(ms, vaddr, phys, dest_vaddr);
    if (error) {
        return error;
    }

    /* Return attributes and permissions. */
    *page_size = ms->page_size;
    *perms = ms->perms;
    *attributes = ms->attributes;

    return OKL4_OK;
}

