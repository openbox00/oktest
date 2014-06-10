/*
 * Description:   very simple kernel memory allocator
 */
#include <l4.h>
#include <kmemory.h>
#include <debug.h>
#include <init.h>
#include <sync.h>
#include <kmem_resource.h>
#include <arch/globals.h>

void kmem_t::init(void * start, void * end)
{
    /* initialize members */
    kmem_free_list = NULL;
    free_chunks = 0;
    /* do the real work */
    free(start, (word_t)end - (word_t)start);
}

/* the stupid version */
void kmem_t::free(void * address, word_t size)
{
    word_t* p;
    word_t* prev, *curr;
    size = max(size, (word_t)KMEM_CHUNKSIZE);

    for (p = (word_t*)address;
         p < ((word_t*)(((word_t)address) + size - KMEM_CHUNKSIZE));
         p = (word_t*) *p)
        *p = (word_t) p + KMEM_CHUNKSIZE; /* write next pointer */

    /* find the place to insert */
    for (prev = (word_t*) (void*)&kmem_free_list, curr = kmem_free_list;
         curr && (address > curr);
         prev = curr, curr = (word_t*) *curr);
    /* and insert there */
    *prev = (word_t) address; *p = (word_t) curr;

    /* update counter */
    free_chunks += (size/KMEM_CHUNKSIZE);
}


/* the stupid version */
void * kmem_t::alloc(word_t size, bool zeroed)
{

    word_t*     prev;
    word_t*     curr;
    word_t*     tmp;
    word_t      i;

    /* round up to the next CHUNKSIZE */
    size = ((size - 1) & ~(KMEM_CHUNKSIZE-1)) + KMEM_CHUNKSIZE;

    for (prev = (word_t*)(void*) &kmem_free_list, curr = kmem_free_list;
         curr;
         prev = curr, curr = (word_t*) *curr)
    {
        /* properly aligned ??? */
        if (!((word_t) curr & (size - 1)))
        {

            tmp = (word_t*) *curr;
            for (i = 1; tmp && (i < (size / KMEM_CHUNKSIZE)); i++)
            {

                if ((word_t) tmp != ((word_t) curr + KMEM_CHUNKSIZE*i))
                {
                    tmp = 0;
                    break;
                };
                tmp = (word_t*) *tmp;
            }
            if (tmp)
            {
                word_t num_chunks = size/KMEM_CHUNKSIZE;

                /* dequeue */
                *prev = (word_t) tmp;
                if (zeroed)
                {
                    /* zero the page */
                    word_t *p = curr;
                    do {
                        *p++ = 0; *p++ = 0; *p++ = 0; *p++ = 0;
                    } while ((word_t)p < (word_t)curr + size);
                }

                /* update counter */
                free_chunks -= num_chunks;

                return curr;
            }
        }
    }
    return NULL;
}

