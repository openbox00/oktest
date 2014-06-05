#include "range_helpers.h"
#include <stdlib.h>
#include <okl4/range.h>
#include <okl4/allocator_attr.h>

void
okl4_range_allocator_free(okl4_range_allocator_t * allocator,
        okl4_range_item_t * victim)
{
    okl4_range_item_t * before_victim;


    RANGE_LIST_ITERATE(before_victim, &allocator->head) {
        if (before_victim->next == victim) {
            break;
        }
    }

    /* Merge range and before_victim. */
    before_victim->total_size += victim->total_size;

    /* Remove the freed victim from the allocator list altogether */
    before_victim->next = victim->next;

#if !defined(NDEBUG)
    /* Poison the victim. */
    victim->base = OKL4_POISON;
    victim->size = OKL4_POISON;
    victim->total_size = OKL4_POISON;
    victim->next = NULL;
#endif
}
