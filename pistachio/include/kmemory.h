/*
 * Description:   Kernel Memory Manager
 */
#ifndef __KMEMORY_H__
#define __KMEMORY_H__

#include <debug.h>
#include <kernel/generic/lib.h>
typedef word_t kmem_group_t;

#define KMEM_CHUNKSIZE  (1024)

class kmem_t
{
    word_t *kmem_free_list;
    word_t free_chunks;

    void free (void * address, word_t size);
    void * alloc (word_t size, bool zeroed);
    void * alloc_aligned (word_t size, word_t alignement, word_t mask, bool zeroed);
public:
    void init (void * start, void * end);
    void free (kmem_group_t * const group, void * address, word_t size);
    void * alloc (kmem_group_t * const group, word_t size, bool zeroed);
    void * alloc_aligned (kmem_group_t * const group, word_t size, word_t alignment,
                          word_t mask, bool zeroed);

    word_t chunks_left(void)
        { return free_chunks; }
    friend class kdb_t;
};


INLINE void * kmem_t::alloc (kmem_group_t * const group, word_t size, bool zeroed)
{
    return alloc (size, zeroed);
}


#endif /* !__KMEMORY_H__ */
