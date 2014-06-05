#include <l4/thread.h>
#include <l4/space.h>
#include <l4/kdebug.h>

#include <okl4/zone.h>
#include <okl4/types.h>
#include <okl4/memsec.h>
#include <okl4/extension.h>
#include <okl4/kspace.h>

#include "pd_helpers.h"

#ifdef ARM_SHARED_DOMAINS
static void
map_window(okl4_kspaceid_t client, okl4_kspaceid_t owner,
        okl4_range_item_t range)
{
    /* Align outwards to 1MB boundaries. */
    range.size += (range.base % OKL4_EXTENSION_ALIGNMENT);
    range.base -= (range.base % OKL4_EXTENSION_ALIGNMENT);
    if (range.size % OKL4_EXTENSION_ALIGNMENT > 0) {
        range.size -= (range.size % OKL4_EXTENSION_ALIGNMENT);
        range.size += OKL4_EXTENSION_ALIGNMENT;
    }

    /*
     * Create the window.
     *
     * We don't check to see if there was an error: If there is an
     * error, we assume it is because of overmapping another window we
     * have already set up.
     */
    (void)_okl4_kspace_share_domain_add_window(client, owner, 0, range);
}
#endif

void
_okl4_extension_premap_mem_container(okl4_extension_t *ext,
        _okl4_mem_container_t *container, okl4_word_t attach_perms)
{
#ifndef ARM_SHARED_DOMAINS

    if (container->type == _OKL4_TYPE_MEMSECTION) {
        _okl4_kspace_premap_memsec(ext->kspace->id,
                (okl4_memsec_t *)container, attach_perms);
    } else if (container->type == _OKL4_TYPE_ZONE) {
        _okl4_kspace_premap_zone(ext->kspace->id,
                (okl4_zone_t *)container, attach_perms);
    } else {
    }

#else /* ARM_SHARED_DOMAINS */

    /* Map a window from the extension space back into the base space. */
    if (container->type == _OKL4_TYPE_MEMSECTION) {
        map_window(ext->kspace->id, ext->base_pd->kspace->id, container->range);
    } else if (container->type == _OKL4_TYPE_ZONE) {
        map_window(ext->kspace->id, ((okl4_zone_t *)container)->kspaceid,
                container->range);
    } else {
    }

#endif /* ARM_SHARED_DOMAINS */
}

/*
 * Premap every memsec already attached to the protection domain into our
 * extension space.
 */
static void
premap_extension_space(okl4_pd_t *pd, okl4_extension_t *ext)
{
    _okl4_mcnode_t *next = pd->mem_container_list;

    while (next != NULL) {
        if (next->payload != &ext->super) {
            _okl4_extension_premap_mem_container(ext,
                    next->payload, next->attach_perms);
        } else {
        }
        next = next->next;
    }
}

/*
 * Attach the given extension to the given pd.
 */
int
okl4_pd_extension_attach(okl4_pd_t *pd, okl4_extension_t *ext)
{
    int error;

    /* Ensure that the extension is appropriate for the PD. */
    if (ext->base_pd != pd) {
        return OKL4_INVALID_ARGUMENT;
    }

    /* Ensure that no other extension is already attached. */
    if (pd->extension != NULL) {
        return OKL4_IN_USE;
    }

    /* Populate the extension space with the mappings in the base space. */
    premap_extension_space(pd, ext);
    /* Add this extension to the PD. */
    ext->node.payload = &ext->super;
    ext->node.attach_perms = L4_FullyAccessible;
    error = _okl4_mem_container_list_insert(&pd->mem_container_list,
            &ext->node);
    if (error) {
        return error;
    }

    /* Note that we are attached. */
    pd->extension = ext;
    ext->attached = 1;

    return 0;
}

