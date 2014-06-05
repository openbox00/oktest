#include <okl4/kclistid_pool.h>

int
okl4_kclistid_allocany(okl4_kclistid_pool_t * pool, okl4_kclistid_t * id)
{
    int ret;
    okl4_bitmap_item_t bitmap_item;

    okl4_bitmap_item_setany(&bitmap_item);
    ret = okl4_bitmap_allocator_alloc(pool, &bitmap_item);
    if (ret != OKL4_OK) {
        return ret;
    }

    id->clist_no = bitmap_item.unit;
    return ret;
}
