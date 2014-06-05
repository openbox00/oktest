#include <okl4/types.h>
#include <okl4/bitmap.h>
#include <okl4/allocator_attr.h>
#include "bitmap_internal.h"

void
okl4_bitmap_allocator_init(okl4_bitmap_allocator_t * allocator,
        okl4_allocator_attr_t * attr)
{
    okl4_word_t i;
    okl4_word_t overflow;
    okl4_word_t bitmap_words;

    if (attr->base == OKL4_ALLOC_ANONYMOUS) {
        allocator->base = 0;
    } else {
        allocator->base = attr->base;
    }
    allocator->size = attr->size;
    allocator->pos_guess = 0;

    bitmap_words = (allocator->size - 1) / OKL4_WORD_T_BIT + 1;
    overflow = (bitmap_words * OKL4_WORD_T_BIT) - allocator->size;

    /* Mark full bitmap range as free. */
    for (i = 0; i < bitmap_words; i++) {
        allocator->data[i] = 0;
    }

    /* Determine the number of spare bits in the last word. */
    overflow = (bitmap_words * OKL4_WORD_T_BIT) - allocator->size;
    if (overflow == 0) {
        return;
    }

    /* We may have more space in our data structure than we are actually able
     * to allocate (because we must allocate in multiples of okl4_word_t size),
     * so mark the extra bits as allocated. */
    allocator->data[bitmap_words - 1] |= (~(okl4_word_t)0 <<
        (OKL4_WORD_T_BIT - overflow));
}
