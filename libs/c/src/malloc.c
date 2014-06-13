/*
 * K&R Malloc 
 *
 * System specifc code should implement `more_core'
 */
#include "k_r_malloc.h"
#include "malloc.h"
#include <stddef.h>             /* For NULL */
#include <stdlib.h>
#include <string.h>             /* For memcpy */

Header __malloc_base;             /* empty list to get started */

void *
malloc(size_t nbytes)
{
    Header *p, *prevp;
    unsigned nunits;

    if (nbytes == 0) {
        return NULL;
    }

    nunits = ((nbytes + sizeof(Header) - 1) / sizeof(Header)) + 1;
    if ((prevp = freep) == NULL) {      /* no free list yet */
        base.s.ptr = freep = prevp = &base;
        base.s.size = 0;
    }
    for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
        if (p->s.size >= nunits) {      /* big enough */
            if (p->s.size == nunits)    /* exactly */
                prevp->s.ptr = p->s.ptr;
            else {              /* allocate tail end */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *)(p + 1);
        }
        if (p == freep) {       /* wrapped around free list */
            if ((p = (Header *)morecore(nunits)) == NULL) {
                return NULL;    /* none left */
            }
        }
    }
}

