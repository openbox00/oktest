#include <okl4/memsec.h>

#define IS_POWER_TWO(x) (((x) & ((x) - 1)) == 0)

void
_okl4_memsec_init(okl4_memsec_t *memsec)
{
    memsec->super.type = _OKL4_TYPE_MEMSECTION;
    memsec->owner = NULL;
}

void
okl4_memsec_init(okl4_memsec_t *memsec, okl4_memsec_attr_t *attr)
{
    _okl4_memsec_init(memsec);

    memsec->super.range = attr->range;
    memsec->perms = attr->perms;
    memsec->page_size = attr->page_size;
    memsec->attributes = attr->attributes;
    memsec->access_callback = attr->access_callback;
    memsec->premap_callback = attr->premap_callback;
    memsec->destroy_callback = attr->destroy_callback;
}

