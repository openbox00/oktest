#include <okl4/kspaceid_pool.h>

int
okl4_kspaceid_allocany(okl4_kspaceid_pool_t * pool, okl4_kspaceid_t * id)
{
    okl4_bitmap_item_t bitmap_item;
    int ret;

    okl4_bitmap_item_setany(&bitmap_item);
    ret = okl4_bitmap_allocator_alloc(pool, &bitmap_item);
    if (ret != OKL4_OK) {
        return ret;
    }
    id->space_no = bitmap_item.unit;
    return ret;
}
