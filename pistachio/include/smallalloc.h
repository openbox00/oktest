/*
 * Description:   Kernel Small Object Allocator
 */
#ifndef __SMALLALLOC_H__
#define __SMALLALLOC_H__

#include <kernel/bitmap.h>
#include <kernel/generic/lib.h>
class small_alloc_block_t;

class small_alloc_t
{
public:
    /* initialize the allocator (max = 0 means unlimited) */
    void init(kmem_group_t * const group, u32_t obj_size, u32_t max_objs = 0);

    /* allocate an object */
    void * allocate(bool zeroed);
private:
    small_alloc_block_t * allocate_block(kmem_group_t * const group);
    void free_block(kmem_group_t * const group, small_alloc_block_t *block);

    void * get_object(small_alloc_block_t * block, word_t index)
        {
            word_t offset = index * obj_size;
            return (void *)((word_t)block - header_offset + offset);
        }

    void * get_block_base(void * object)
        {
            return (void *)((word_t)object & ~(SMALL_OBJECT_BLOCKSIZE - 1UL));
        }

    void * get_block_base(small_alloc_block_t * block)
        {
            return (void *)((word_t)block - header_offset);
        }

    small_alloc_block_t * get_block(void * base)
        {
            return (small_alloc_block_t *)((word_t)base + header_offset);
        }


private:
    small_alloc_block_t * head;
    small_alloc_block_t * first_free;
    kmem_group_t * mem_group;

    u32_t num;
    u32_t max_objs;

    u16_t obj_size;
    u16_t objs_per_block;
    u16_t header_offset;
};

class small_alloc_block_t
{
public:
    static word_t header_size(word_t obj_size)
        {
            return (2 + BITMAP_SIZE(SMALL_OBJECT_BLOCKSIZE/obj_size)) * sizeof(word_t);
        }

public:
    bitmap_t * get_bitmap(void)
        {
            return &bitmap[0];
        }
public:
    small_alloc_block_t * next;
    word_t id;
    bitmap_t bitmap[1];
};


#endif
