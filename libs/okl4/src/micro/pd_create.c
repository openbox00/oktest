#include "pd_helpers.h"

int
_okl4_utcb_memsec_lookup(okl4_memsec_t *memsec, okl4_word_t vaddr,
        okl4_physmem_item_t *map_item, okl4_word_t *dest_vaddr)
{
    return 1;
}

int
_okl4_utcb_memsec_map(okl4_memsec_t *memsec, okl4_word_t vaddr,
        okl4_physmem_item_t *map_item, okl4_word_t *dest_vaddr)
{
       return 1; 
}

void
_okl4_utcb_memsec_destroy(okl4_memsec_t *ms)
{
}

