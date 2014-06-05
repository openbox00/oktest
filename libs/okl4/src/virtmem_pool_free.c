#include <okl4/virtmem_pool.h>

void
okl4_virtmem_pool_free(okl4_virtmem_pool_t *pool,
        okl4_virtmem_item_t *range)
{
    okl4_range_allocator_free(&pool->allocator, range);
}

