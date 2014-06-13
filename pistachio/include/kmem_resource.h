/*
 * Description:   Kernel Resource Manager
 */
#ifndef __KMEM_RESOURCE_H__
#define __KMEM_RESOURCE_H__

#include <kmemory.h>
#include <smallalloc.h>

extern word_t pre_tcb_init;
class space_t;

/*
 * All small_alloc pool and kmem pool should be registered here,
 * note that small_alloc pool always has smaller numbers, that
 * is because they are used as index to small_alloc_pools.
 * When adding or deleting new entry, don't forget to add/delete
 * entry in __kmem_group_names[] as well.
 */
typedef enum {
    /* start of small_alloc pools */
    kmem_group_mutex,
    kmem_group_space,
    kmem_group_tcb,
    kmem_group_l0_allocator,
    kmem_group_l1_allocator,
    /* end of small_alloc pools, below are kmem pools */
    kmem_group_clist,
    kmem_group_clistids,
    kmem_group_root_clist,
    kmem_group_ll,
    kmem_group_misc,
    kmem_group_mutexids,
    kmem_group_pgtab,
    kmem_group_resource,
    kmem_group_spaceids,
    kmem_group_stack,
    kmem_group_trace,
    kmem_group_utcb,
    kmem_group_irq,
    kmem_group_physseg_list,
    max_kmem_group
} kmem_group_e;

#define MAX_KMEM_GROUP  (word_t) max_kmem_group
#define MAX_KMEM_SMALL_ALLOC_GROUP kmem_group_clist

class kmem_resource_t
{
private:
    kmem_t          heap;  /* per resource pool heap */
    small_alloc_t   small_alloc_pools[MAX_KMEM_SMALL_ALLOC_GROUP];
    kmem_group_t    kmem_groups[MAX_KMEM_GROUP];

    void init_heap(void * start, void *end)
    {
        heap.init(start, end);
    }
    void init_kmem_groups(void)
    {
        word_t i;
        for (i = 0; i < MAX_KMEM_GROUP; i++)
        {
            kmem_groups[i] = 0;
        }
    }
    void arch_init_small_alloc_pools(void);
    void init_small_alloc_pools(void);

public:
    /* Disable copy constructor */
    kmem_resource_t(const kmem_resource_t&);
    /* Disable operator = */
    kmem_resource_t& operator= (const kmem_resource_t&);

    bool is_small_alloc_group(kmem_group_e group) {
        bool ret;
        ret = (group == kmem_group_space) || (group == kmem_group_tcb) ||
             (group == kmem_group_mutex);
        ret = ret || (group == kmem_group_l0_allocator) ||
             (group == kmem_group_l1_allocator);
        return ret;
    }

    void init(void *end);

    void * alloc(kmem_group_e group, bool zeroed);
    void * alloc(kmem_group_e group, word_t size, bool zeroed);
    void * alloc_aligned(kmem_group_e group, word_t size,
                         word_t alignment, word_t mask, bool zeroed);
    void free(kmem_group_e group, void * address, word_t size = 0);

    friend class small_alloc_t;
    friend class kdb_t;
};

extern kmem_resource_t * get_current_kmem_resource(void);

#endif /* !__KMEM_RESOURCE_H__ */
