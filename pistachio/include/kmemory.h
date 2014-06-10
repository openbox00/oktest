/*
 * Description:   Kernel Memory Manager
 */
#ifndef __KMEMORY_H__
#define __KMEMORY_H__

#include <debug.h>
#include <sync.h>
#include <kernel/generic/lib.h>

#if defined(CONFIG_KMEM_TRACE)
#include <kdb/macro_set.h>
class kmem_group_t
{
public:
    word_t      mem;
    const char *      name;
};

extern const char* __kmem_group_names[];

#else
typedef word_t kmem_group_t;
#endif

#define KMEM_CHUNKSIZE  (1024)

class kmem_t
{
    word_t *kmem_free_list;
    word_t free_chunks;

    void free (void * address, word_t size);
    void * alloc (word_t size, bool zeroed);
    void * alloc_aligned (word_t size, word_t alignement, word_t mask, bool zeroed);
#if defined(CONFIG_KMEM_DEBUG)
    void check_free(void * address, word_t size);
#endif

public:
    void init (void * start, void * end);
    void free (kmem_group_t * const group, void * address, word_t size);
    void * alloc (kmem_group_t * const group, word_t size, bool zeroed);
    void * alloc_aligned (kmem_group_t * const group, word_t size, word_t alignment,
                          word_t mask, bool zeroed);

    void add (void * address, word_t size)
        { free (address, size); }

    word_t chunks_left(void)
        { return free_chunks; }
    friend class kdb_t;
};


#if defined(CONFIG_KMEM_TRACE)

INLINE void * kmem_t::alloc (kmem_group_t * const group, word_t size, bool zeroed)
{
    void * ret = alloc (size, zeroed);
    if (ret)
        group->mem += max(size, (word_t)KMEM_CHUNKSIZE);
    return ret;
}

INLINE void * kmem_t::alloc_aligned (kmem_group_t * const group, word_t size,
                                     word_t alignment, word_t mask, bool zeroed)
{
    void * ret = alloc_aligned (size, alignment, mask, zeroed);
    if (ret)
        group->mem += max(size, (word_t)KMEM_CHUNKSIZE);
    return ret;
}

INLINE void kmem_t::free (kmem_group_t * const group, void * address, word_t size)
{
    ASSERT (NORMAL, group->mem >= size);
    group->mem -= max(size, (word_t)KMEM_CHUNKSIZE);
    free (address, size);
}

#else /* !CONFIG_KMEM_TRACE */

INLINE void * kmem_t::alloc (kmem_group_t * const group, word_t size, bool zeroed)
{
    return alloc (size, zeroed);
}

INLINE void * kmem_t::alloc_aligned (kmem_group_t * const group, word_t size,
                                     word_t alignment, word_t mask, bool zeroed)
{
    return alloc_aligned (size, alignment, mask, zeroed);
}

INLINE void kmem_t::free (kmem_group_t * const group, void * address, word_t size)
{
    free (address, size);
}

#endif

#if defined(CONFIG_KMEM_DEBUG)
/* Borrow the values used in Linux */
#define KMEM_POISON_ALLOC               0x5a
#define KMEM_POISON_FREE                0x6b
void poison_area(void * addr, word_t size, unsigned char poison_value);
void check_poisoned_area(void * addr, word_t size,
                         unsigned char poison_value, void *obj_addr);
#endif



#endif /* !__KMEMORY_H__ */
