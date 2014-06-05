#include "range_helpers.h"
#include <stdlib.h>
#include <okl4/range.h>
#include <okl4/allocator_attr.h>
#include <stdio.h>

static int named_allocation(okl4_range_allocator_t * allocator,
        okl4_range_item_t * range);

static int anonymous_allocation(okl4_range_allocator_t * allocator,
        okl4_range_item_t * range);

/* Implementation */

int okl4_range_allocator_alloc(okl4_range_allocator_t * allocator,
        okl4_range_item_t * range)
{
    if (range_is_anonymous(range)) {
        return anonymous_allocation(allocator, range);
    } else {
        return named_allocation(allocator, range);
    }
}

static int
named_allocation(okl4_range_allocator_t * allocator,
        okl4_range_item_t * range)
{
    okl4_range_item_t * victim;
    okl4_word_t victim_free_size;


    /*
     * We search for the range item (node) that we allocate from, and call
     * that the victim.
     */
    RANGE_LIST_ITERATE(victim, &allocator->head) {
        if (range_free_hasvalue(victim, range->base)) {
            break;
        }
    }

    if (victim == NULL) {
        return OKL4_ALLOC_EXHAUSTED;
    }

    /*
     * Remove the free region that is being allocated out of victim,
     * if it is large enough.
     */
    victim_free_size = victim->base + victim->total_size - range->base;
    if (victim_free_size < range->size) {
        return OKL4_IN_USE;
    }
    range->total_size = victim->base + victim->total_size - range->base;
    victim->total_size -= range->total_size;

    /* Insert range item object into list */
    range->next = victim->next;
    victim->next = range;

    return OKL4_OK;
}

static int
anonymous_allocation(okl4_range_allocator_t * allocator,
        okl4_range_item_t * range)
{
    okl4_word_t range_size = range->size;
    okl4_range_item_t * victim;

    /* Find the first range item that has a sufficient free region */
    RANGE_LIST_ITERATE(victim, &allocator->head) {
        if (range_free_getsize(victim) >= range_size) {
            break;
        }
    }
    if (victim == NULL) {
        return OKL4_IN_USE;
    }

    /*
     * Remove the free region that is being allocated out of victim.
     * Write allocated region to the range item object.  There is no
     * free region.
     */
    victim->total_size -= range_size;
    range->base = victim->base + victim->total_size;
    range->total_size = range_size;

    /* Insert range item into appropriate list */
    range->next = victim->next;
    victim->next = range;

    return OKL4_OK;
}
