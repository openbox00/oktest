#include <okl4/kspaceid_pool.h>

void
okl4_kspaceid_free(okl4_kspaceid_pool_t * pool, okl4_kspaceid_t id)
{
    okl4_bitmap_item_t bitmap_item;
    bitmap_item.unit = id.space_no;
    okl4_bitmap_allocator_free(pool, &bitmap_item);
}
