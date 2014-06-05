#include <okl4/types.h>
#include <okl4/bitmap.h>
#include <okl4/allocator_attr.h>
#include "bitmap_internal.h"

void
okl4_bitmap_allocator_free(okl4_bitmap_allocator_t * allocator,
        okl4_bitmap_item_t * item)
{
    okl4_word_t unit;

    /* Adjust and free. */
    unit = item->unit - allocator->base;
    OKL4_CLEAR_BIT(allocator->data[unit / OKL4_WORD_T_BIT],
            unit % OKL4_WORD_T_BIT);
}
