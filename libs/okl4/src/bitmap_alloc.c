#include <okl4/types.h>
#include <okl4/bitmap.h>
#include <okl4/allocator_attr.h>
#include "bitmap_internal.h"

static int anonymous_allocation(okl4_bitmap_allocator_t * allocator,
        okl4_bitmap_item_t * item);

static int named_allocation(okl4_bitmap_allocator_t * allocator,
        okl4_bitmap_item_t * item);

int
okl4_bitmap_allocator_alloc(okl4_bitmap_allocator_t * allocator,
        okl4_bitmap_item_t * item)
{

    if (item->unit != OKL4_ALLOC_ANONYMOUS) {
        return named_allocation(allocator, item);
    }
    else {
        return anonymous_allocation(allocator, item);
    }
}

/* Request any unit. */
static int
anonymous_allocation(okl4_bitmap_allocator_t * allocator,
        okl4_bitmap_item_t * item)
{
    okl4_word_t i;
    int pos;
    okl4_word_t bitmap_words = (allocator->size + OKL4_WORD_T_BIT - 1) / OKL4_WORD_T_BIT;

    /* Search for a free unit. */
    for (i = allocator->pos_guess; i < bitmap_words; i++) {
        if (allocator->data[i] != ~(okl4_word_t)0) {
            goto found;
        }
    }
    /* Other possible locations for availble units. */
    for (i = 0; i < allocator->pos_guess; i++) {
        if (allocator->data[i] != ~(okl4_word_t)0) {
            goto found;
        }
    }

    return OKL4_ALLOC_EXHAUSTED;

found:

    /* Update pos_guess to point to a likely location for free units. */
    allocator->pos_guess = i;

    pos = find_first_free(allocator->data[i]);

    /* Allocate and return. */
    OKL4_SET_BIT(allocator->data[i], pos);
    item->unit = (i * OKL4_WORD_T_BIT) + (okl4_word_t)pos + allocator->base;

    return OKL4_OK;
}

/* Request for a specific unit. */
static int
named_allocation(okl4_bitmap_allocator_t * allocator,
        okl4_bitmap_item_t * item)
{
    okl4_word_t unit, unit_word, unit_bit;

    /* Check that the requested unit is within range. */
    if (item->unit < allocator->base ||
            item->unit >= allocator->base + allocator->size) {
        return OKL4_OUT_OF_RANGE;
    }

    /* Make adjustment for the fact that the first element in the bitmap is
     * always at position zero, not allocator->base */
    unit = item->unit - allocator->base;
    unit_word = unit / OKL4_WORD_T_BIT;
    unit_bit  = unit % OKL4_WORD_T_BIT;

    /* Check if requested unit has already been allocated. */
    if (OKL4_TEST_BIT(allocator->data[unit_word], unit_bit)) {
        return OKL4_IN_USE;
    }

    /* Allocate. */
    OKL4_SET_BIT(allocator->data[unit_word], unit_bit);

    return OKL4_OK;
}


