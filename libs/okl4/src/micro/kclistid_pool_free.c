#include <okl4/kclistid_pool.h>

void
okl4_kclistid_free(okl4_kclistid_pool_t * pool, okl4_kclistid_t id)
{
    okl4_bitmap_item_t bitmap_item;
    bitmap_item.unit = id.clist_no;
    okl4_bitmap_allocator_free(pool, &bitmap_item);
}
