#include <okl4/types.h>

#if defined(OKL4_KERNEL_MICRO)
extern void *_okl4_utcb_memsec_lookup;
extern void *_okl4_utcb_memsec_map;
extern void *_okl4_utcb_memsec_destroy;
extern void *okl4_api_version;
void *okl4_forced_symbols[] = {
    &_okl4_utcb_memsec_lookup,
    &_okl4_utcb_memsec_map,
    &_okl4_utcb_memsec_destroy,
    &okl4_api_version
};
#endif /* OKL4_KERNEL_MICRO */

