/*
 * Description:  Kernel Resource Manager
 */

#include <l4.h>
#include <kmem_resource.h>
#include <space.h>
#include <tcb.h>

kmem_resource_t * get_current_kmem_resource()
{
    kmem_resource_t *ret;
    space_t * current_space;

    /* if current_tcb is not setup, use kernel space. */
    if (pre_tcb_init)
    {
        current_space = get_kernel_space();
    }
    else
    {
        current_space = get_current_space();
    }
    if (current_space == NULL) current_space = get_kernel_space();

    ret = current_space->get_kmem_resource();
    return ret;
}

void kmem_resource_t::init_small_alloc_pools(void)
{
    /* init small alloc pool with no max limit */

    small_alloc_pools[kmem_group_space].init(&kmem_groups[kmem_group_space], sizeof(space_t));
    small_alloc_pools[kmem_group_tcb].init(&kmem_groups[kmem_group_tcb],KTCB_SIZE);
    
    arch_init_small_alloc_pools();
}

void kmem_resource_t::init(void *end)
{
    init_heap((void *)((word_t)this + KMEM_CHUNKSIZE), end);
    init_kmem_groups();
    init_small_alloc_pools();
}

void * kmem_resource_t::alloc(kmem_group_e group, bool zeroed)
{
    void * ret;
    ret = small_alloc_pools[group].allocate(zeroed);
    return ret;
}

void * kmem_resource_t::alloc(kmem_group_e group, word_t size, bool zeroed)
{
    void * ret;
    ret = heap.alloc(&kmem_groups[group], size, zeroed);
    return ret;
}

void kmem_resource_t::free(kmem_group_e group, void * address, word_t size)
{
}

