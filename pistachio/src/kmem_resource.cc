/*
 * Description:  Kernel Resource Manager
 */

#include <l4.h>
#include <kmem_resource.h>
#include <space.h>
#include <tcb.h>

#if defined(CONFIG_KMEM_TRACE)
const char* __kmem_group_names[MAX_KMEM_GROUP] = {
    "kmem_mutex",
    "kmem_space",
    "kmem_tcb",
    "kmem_l0_allocator",
    "kmem_l1_allocator",
    "kmem_clist",
    "kmem_clistids",
    "kmem_root_clist",
    "kmem_ll",
    "kmem_misc",
    "kmem_mutexids",
    "kmem_pgtab",
    "kmem_resource",
    "kmem_spaceids",
    "kmem_stack",
    "kmem_trace",
    "kmem_utcb",
    "kmem_irq",
    "kmem_physseg_list"
};
#endif

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
    /* We should've already checked that we have the privilege
     * (have the access to kmem_resource object)
     * to create any kernel object at the beginning of the
     * system call, so that we add assertion here to double
     * check instead of return NULL pointer to indicate failure.
     */

    ASSERT(ALWAYS, ret != NULL);
    return ret;
}

void kmem_resource_t::init_small_alloc_pools(void)
{
    /* init small alloc pool with no max limit */
    small_alloc_pools[kmem_group_space].init(&kmem_groups[kmem_group_space],
            sizeof(space_t));
    small_alloc_pools[kmem_group_tcb].init(&kmem_groups[kmem_group_tcb],
            KTCB_SIZE);
    small_alloc_pools[kmem_group_mutex].init(&kmem_groups[kmem_group_mutex],
            sizeof(mutex_t));
    arch_init_small_alloc_pools();
}

void kmem_resource_t::init(void *end)
{
#if defined(CONFIG_DEBUG)
    word_t size = (word_t)end - (word_t)this;
#if defined(CONFIG_KDB_CONS)
    this->next = NULL;
    if (__resources == NULL) {
        __resources = this;
    } else {
        kmem_resource_t *current = __resources;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = this;
    }
#endif
#endif
    ASSERT(ALWAYS, (size % KMEM_CHUNKSIZE) == 0);
    ASSERT(ALWAYS, sizeof(kmem_resource_t) <= KMEM_CHUNKSIZE);
    lock.init();
    init_heap((void *)((word_t)this + KMEM_CHUNKSIZE), end);
    init_kmem_groups();
    init_small_alloc_pools();
}

void * kmem_resource_t::alloc(kmem_group_e group, bool zeroed)
{
    void * ret;
    lock.lock();
    ASSERT(ALWAYS, is_small_alloc_group(group));
    ret = small_alloc_pools[group].allocate(zeroed);
    lock.unlock();
    return ret;
}

void * kmem_resource_t::alloc(kmem_group_e group, word_t size, bool zeroed)
{
    void * ret;
    lock.lock();
    ASSERT(ALWAYS, !is_small_alloc_group(group));
    ret = heap.alloc(&kmem_groups[group], size, zeroed);
    lock.unlock();
    return ret;
}

void kmem_resource_t::free(kmem_group_e group, void * address, word_t size)
{
    lock.lock();
    if (size == 0)
    {
        ASSERT(ALWAYS, is_small_alloc_group(group));
        small_alloc_pools[group].free(address);
    }
    else
    {
        ASSERT(ALWAYS, !is_small_alloc_group(group));
        heap.free(&kmem_groups[group], address, size);
    }
    lock.unlock();
}

