#ifndef __PD_HELPERS_H__
#define __PD_HELPERS_H__

#include <okl4/types.h>

/* Ensure that 'b' is true if and only if 'a' is true. */
#define IFF(a, b) (!!(a) == !!(b))

/* If 'a' is true, ensure 'b' is true. */
#define IMPLIES(a, b) (!(a) || (b))

/* Either 'a' is true or 'b' is true but not both. */
#define XOR(a, b) ((!(a) && (b)) || ((a) && !(b)))

/* Round the given number 'n' up to the next multiple of 'd'. */
#define ROUND_UP(n, d) ((((n) + (d) - 1) / (d)) * (d))

/*
 * Insert a generic memory container (e.g. memsec or zone) into the specified
 * list. Also tests that the address of the new element does not conflict with
 * addresses of existing elements.
 */
int _okl4_mem_container_list_insert(_okl4_mcnode_t **head,
        _okl4_mcnode_t *node);

/*
 * Remove a generic memory container (e.g. memsec or zone) from the specified
 * list. Returns a pointer to the node that the container was held in.
 */
_okl4_mcnode_t *_okl4_mem_container_list_remove(_okl4_mcnode_t **head,
        _okl4_mem_container_t *container);

/*
 * Determine if two containers overlap.
 */
int _okl4_is_container_overlap(_okl4_mem_container_t *a,
        _okl4_mem_container_t *b);

/*
 * Determine if container 'small' is contained completely within container
 * 'large'.
 */
int _okl4_is_contained_in(_okl4_mem_container_t *small,
        _okl4_mem_container_t *large);

/*
 * Determine if the point 'a' falls within the interval '[x,y)'.
 */
int _okl4_is_point_in_range(okl4_word_t a, okl4_word_t x, okl4_word_t y);

/*
 * Premap the given memsec into the kspace with given kspaceid.
 */
void _okl4_kspace_premap_memsec(okl4_kspaceid_t kspaceid, okl4_memsec_t *memsec,
                                okl4_word_t perm_mask);

/*
 * Unmap the given memsec from the kspace with given kspaceid.
 */
void _okl4_kspace_unmap_memsec(okl4_kspaceid_t kspaceid, okl4_memsec_t *memsec);

/*
 * Premap the given zone into the kspace with the given kspaceid.
 */
void _okl4_kspace_premap_zone(okl4_kspaceid_t kspaceid, okl4_zone_t *zone,
          okl4_word_t perms);

/*
 * Unmap the given zone from the kspace with the given ksapceid.
 */
void _okl4_kspace_unmap_zone(okl4_kspaceid_t kspaceid, okl4_zone_t *zone);

/*
 * Given a container list, find the container that the given 'vaddr' falls in.
 */
_okl4_mem_container_t * _okl4_get_container_from_vaddr(_okl4_mcnode_t *first,
        okl4_word_t vaddr, _okl4_mem_container_type_t *type,
        okl4_word_t *attach_perms);

/* 
 * UTCB memsection callbacks.
 * */
int
_okl4_utcb_memsec_lookup(okl4_memsec_t *memsec, okl4_word_t vaddr,
        okl4_physmem_item_t *map_item, okl4_word_t *dest_vaddr);

int
_okl4_utcb_memsec_map(okl4_memsec_t *memsec, okl4_word_t vaddr,
        okl4_physmem_item_t *map_item, okl4_word_t *dest_vaddr);

void
_okl4_utcb_memsec_destroy(okl4_memsec_t *ms);

#endif /* ! __PD_HELPERS_H__ */

