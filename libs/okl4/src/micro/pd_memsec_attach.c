#include <okl4/types.h>
#include <okl4/pd.h>
#include <okl4/extension.h>
#include <okl4/memsec.h>

#include "pd_helpers.h"

/**
 * Attach the given memsec to the given protection domain.
 */
int
okl4_pd_memsec_attach(okl4_pd_t *pd, okl4_memsec_t *memsec)
{
    int error;

    /* Insert memsec node into pd list of memory containers. */
    memsec->list_node.payload = &memsec->super;
    memsec->list_node.attach_perms = L4_FullyAccessible;
    error = _okl4_mem_container_list_insert(&pd->mem_container_list,
            &memsec->list_node);
    if (error) {
        return error;
    }

    /* Track this pd as the owner. */
    memsec->owner = pd;

    /* Perform lookups on all the pages, premapping anything into the address
     * range as appropriate. */
    _okl4_kspace_premap_memsec(pd->kspace->id, memsec, L4_FullyAccessible);

    /* If we have an extension, premap it into there as well. */
    if (pd->extension != NULL) {
        _okl4_extension_premap_mem_container(pd->extension,
                (_okl4_mem_container_t *)memsec, L4_FullyAccessible);
    }

    return OKL4_OK;
}

