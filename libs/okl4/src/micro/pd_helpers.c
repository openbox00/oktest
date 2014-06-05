#include <okl4/kspace.h>
#include <okl4/pd.h>
#include <okl4/memsec.h>
#include <okl4/zone.h>

#include "pd_helpers.h"

/*
 * Determine if the point 'a' falls within the interval '[x,y)'.
 */
int
_okl4_is_point_in_range(okl4_word_t a, okl4_word_t x, okl4_word_t y)
{
    return ((a >= x) && (a < y));
}

/*
 * Determine if the two given ranges overlap each other.
 *
 * Two ranges overlap if the start of memsec 'a' falls within the region
 * covered by 'b', or vise-versa.
 */
static int
is_range_overlap(okl4_range_item_t *a, okl4_range_item_t *b)
{
    return _okl4_is_point_in_range(a->base,
            b->base, b->base + b->size)
            || _okl4_is_point_in_range(b->base,
                    a->base, a->base + a->size);
}

/*
 * Determine if two containers overlap.
 */
int
_okl4_is_container_overlap(_okl4_mem_container_t *a, _okl4_mem_container_t *b)
{
    return is_range_overlap(&a->range, &b->range);
}

/*
 * Determine if container 'small' is contained completely within container
 * 'large'.
 */
int
_okl4_is_contained_in(_okl4_mem_container_t *small,
        _okl4_mem_container_t *large)
{
    okl4_word_t l_start = large->range.base;
    okl4_word_t l_end = large->range.base + large->range.size;
    okl4_word_t s_start = small->range.base;
    okl4_word_t s_end = small->range.base + small->range.size;

    return l_start <= s_start && l_end >= s_end;
}

/*
 * Insert container at the correct place within a list of containers. Check that
 * the new container will not overlap with any others already in the list.
 */
int
_okl4_mem_container_list_insert(_okl4_mcnode_t **head,
        _okl4_mcnode_t *new_node)
{
    _okl4_mcnode_t *current;
    _okl4_mcnode_t *previous;


    /* Find the correct spot for the new container in our list. Sorted by
     * address. */
    previous = NULL;
    current = *head;
    while (current != NULL && current->payload->range.base <
            new_node->payload->range.base) {
        previous = current;
        current = current->next;
    }

    /* Ensure that we don't overlap with any other memsection. We could either
     * be overlapping with 'previous' or 'next'. */
    if (current &&
            _okl4_is_container_overlap(current->payload, new_node->payload)) {
        return OKL4_ALLOC_EXHAUSTED;
    }
    if (previous &&
            _okl4_is_container_overlap(previous->payload, new_node->payload)) {
        return OKL4_ALLOC_EXHAUSTED;
    }

    /* Add us to the list of attached nodes. */
    if (previous) {
        previous->next = new_node;
    } else {
        *head = new_node;
    }
    new_node->next = current;

    return OKL4_OK;
}

/**
 * Remove the node with the given container from the given list of nodes.
 * Returns a pointer to the node that the container was held in.
 */

_okl4_mcnode_t *_okl4_mem_container_list_remove(_okl4_mcnode_t **head,
        _okl4_mem_container_t *container)
{
    _okl4_mcnode_t *previous;
    _okl4_mcnode_t *current;

    /* Find the given container in our list of memory container nodes. */
    previous = NULL;
    current = *head;
    while (current != NULL) {
        /* Have we found our node? */
        if (current->payload == container) {
            break;
        }

        /* Otherwise, keep searching. */
        previous = current;
        current = current->next;
    }

    /* Did we fail to find the container? */
    if (current == NULL) {
        return NULL;
    }

    /* Remove it from the list. */
    if (previous == NULL) {
        *head = current->next;
    } else {
        previous->next = current->next;
    }

    /* Clear up the removed node. */
    current->next = NULL;

    return current;
}

/*
 * Premap the given memsec into the provided kspace.
 */
void
_okl4_kspace_premap_memsec(okl4_kspaceid_t kspaceid, okl4_memsec_t *memsec,
                           okl4_word_t perm_mask)
{
    okl4_word_t current_vaddr = memsec->super.range.base;

    /* Iterate over every page in the vrange. */
    while (current_vaddr < memsec->super.range.base +
            memsec->super.range.size) {
        int error;
        okl4_kspace_map_attr_t attr;
        okl4_virtmem_item_t range;
        okl4_physmem_item_t phys;
        okl4_word_t dest_vaddr;
        okl4_word_t perms;
        okl4_word_t attributes;
        okl4_word_t dummy;

        /* Determine if we should premap this virtual page. */
        int no_map = okl4_memsec_premap(memsec, current_vaddr, &phys,
                &dest_vaddr, &dummy, &perms, &attributes);

        /* If no mapping should be done, move onto the next page. */
        if (no_map) {
            current_vaddr += memsec->page_size;
            continue;
        }

        /* Perform the mapping. */
        okl4_range_item_setrange(&range, dest_vaddr, phys.range.size);
        okl4_kspace_map_attr_init(&attr);
        okl4_kspace_map_attr_setperms(&attr, perms & perm_mask);
        okl4_kspace_map_attr_setattributes(&attr, attributes);
        okl4_kspace_map_attr_setpagesize(&attr, memsec->page_size);
        okl4_kspace_map_attr_settarget(&attr, &phys);
        okl4_kspace_map_attr_setrange(&attr, &range);
        error = _okl4_kspace_mapbyid(kspaceid, &attr);

        /* Move to the end of this mapping. */
        current_vaddr += phys.range.size;
    }
}

/*
 * Unmap the given memsec from the provided kspace.
 */
void
_okl4_kspace_unmap_memsec(okl4_kspaceid_t kspaceid, okl4_memsec_t *memsec)
{
    okl4_kspace_unmap_attr_t attr;
    int error;

    /* Perform the unmap. */
    okl4_kspace_unmap_attr_init(&attr);
    okl4_kspace_unmap_attr_setpagesize(&attr, memsec->page_size);
    okl4_kspace_unmap_attr_setrange(&attr, &memsec->super.range);
    error = _okl4_kspace_unmapbyid(kspaceid, &attr);
}

/*
 * Premap the given zone into the kspace with the provided kspaceid.
 */
void
_okl4_kspace_premap_zone(okl4_kspaceid_t kspaceid,
        okl4_zone_t *zone, okl4_word_t perms_mask)
{
    _okl4_mcnode_t *next;

    /* Map each memsec in the zone. */
    for (next = zone->mem_container_list; next != NULL; next = next->next) {
        _okl4_kspace_premap_memsec(kspaceid, (okl4_memsec_t *)next->payload,
                perms_mask);
    }
}

/*
 * Unmap the given zone from the kspace with the provided kspaceid.
 */
void
_okl4_kspace_unmap_zone(okl4_kspaceid_t kspaceid, okl4_zone_t *zone)
{
    _okl4_mcnode_t *next;

    /* Unmap each memsec in the zone. */
    for (next = zone->mem_container_list; next != NULL; next = next->next) {
        _okl4_kspace_unmap_memsec(kspaceid, (okl4_memsec_t *)next->payload);
    }
}

/*
 * Return the container the given 'vaddr' falls into.
 *
 * 'type' returns the container type.
 */
_okl4_mem_container_t *
_okl4_get_container_from_vaddr(_okl4_mcnode_t *first, okl4_word_t vaddr,
        _okl4_mem_container_type_t *type, okl4_word_t *attach_perms)
{
    /* Iterate over the containers in the list until we find the one 'vaddr'
     * falls in.  */
    _okl4_mcnode_t *next = first;
    while (next != NULL) {
        if (_okl4_is_point_in_range(vaddr, next->payload->range.base,
                    next->payload->range.base +
                    next->payload->range.size - 1)) {
            /* Found it. */
            *attach_perms = next->attach_perms;
            *type = next->payload->type;
            return next->payload;
        }
        next = next->next;
    }

    /* If we reach here, no such container exists. */
    return NULL;
}

