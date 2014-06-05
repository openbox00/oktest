#include <okl4/virtmem_pool.h>

int
okl4_virtmem_pool_alloc(okl4_virtmem_pool_t *pool,
        okl4_virtmem_item_t *range)
{
    return okl4_range_allocator_alloc(&pool->allocator, range);
}

