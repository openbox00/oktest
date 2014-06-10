/*
 * Description:   Kernel Small Object Allocator
 */
#include <kmemory.h>
#include <smallalloc.h>
#include <kmem_resource.h>

small_alloc_block_t*
small_alloc_t::allocate_block(kmem_group_t * const group)
{
    word_t length = objs_per_block;
    void * block_base = get_current_kmem_resource()->heap.alloc(group, SMALL_OBJECT_BLOCKSIZE, false);
    if (!block_base)
        return NULL;
    small_alloc_block_t *block = get_block(block_base);
    if (!head || head->id != 0)
    {
        block->next = head;
        head = block;
        if (max_objs && (length >= max_objs)) {
            length = max_objs;
        }
        bitmap_init(block->get_bitmap(), length, true);
        return block;
    }
    small_alloc_block_t * current = head;
    word_t next_id = current->id + objs_per_block;
    while (current->next)
    {
        current = current->next;
        if (current->id != next_id) {
            block->next = current->next;
            current->next = block;

            block->id = next_id;
            bitmap_init(block->get_bitmap(), length, true);
            return block;
        }
        next_id += objs_per_block;
    }
    block->id = next_id;
    current->next = block;
    block->next = 0;

    if (max_objs && (length + next_id >= max_objs)) {
        length = max_objs - next_id;
    }
    bitmap_init(block->get_bitmap(), length, true);
    return block;
}
void * small_alloc_t::allocate(bool zeroed)
{
    if (max_objs && (num >= max_objs)) {
        return NULL;
    }
    if (EXPECT_FALSE(!first_free)) {
        first_free = allocate_block(mem_group);
        if (!first_free){
            return NULL;
        }
    }
    int position = bitmap_findfirstset(first_free->get_bitmap(), objs_per_block);
    void * object = get_object(first_free, position);
    bitmap_clear(first_free->get_bitmap(), position);
    while (bitmap_findfirstset(first_free->get_bitmap(), objs_per_block) == -1)
    {
        first_free = first_free->next;
        if (!first_free)
            break;
    }
    num++;
    return object;
}
void small_alloc_t::init(kmem_group_t * const group, u32_t object_size, u32_t max_objects)
{
    head = NULL;
    first_free = NULL;
    num = 0;
    this->obj_size = object_size;
    this->max_objs = max_objects;
    this->mem_group = group;
    word_t header_size = small_alloc_block_t::header_size(obj_size);
    word_t extra_space = SMALL_OBJECT_BLOCKSIZE % obj_size;
    if (extra_space >= header_size) {
        objs_per_block = SMALL_OBJECT_BLOCKSIZE / obj_size;
    } else {
        objs_per_block = (SMALL_OBJECT_BLOCKSIZE / obj_size) -
            (header_size - extra_space) / obj_size - 1;
    }
    this->header_offset = objs_per_block * obj_size;
}

