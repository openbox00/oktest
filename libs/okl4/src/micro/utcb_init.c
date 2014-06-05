#include <okl4/utcb.h>
#include <okl4/range.h>

void
okl4_utcb_area_init(okl4_utcb_area_t *utcb_area,
        okl4_utcb_area_attr_t *attr)
{
    okl4_word_t num_utcbs;
    okl4_allocator_attr_t  utcb_alloc_attr;


    /* Initialize the virtual memory item */
    utcb_area->utcb_memory = attr->utcb_range;

    /* The number of utcbs can be used to initialize the bitmap
     * allocator which requires the allocator attributes to be
     * set. */
    num_utcbs = attr->utcb_range.size / sizeof(okl4_utcb_t);
    okl4_allocator_attr_init(&utcb_alloc_attr);
    okl4_allocator_attr_setsize(&utcb_alloc_attr, num_utcbs);

    /* Initialize the bitmap allocator */
    okl4_bitmap_allocator_init(&utcb_area->utcb_allocator,
            &utcb_alloc_attr);
}
