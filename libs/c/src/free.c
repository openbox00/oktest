#include "k_r_malloc.h"
#include "malloc.h"
#include <stddef.h>
#include <stdlib.h>

void
free(void *ap)
{
    Header *bp, *p;

    if (ap == NULL) {
        return;
    }
    if (freep == NULL) {        /* no free list yet */
        base.s.ptr = freep = &base;
        base.s.size = 0;
    }

    bp = (Header *)ap - 1;      /* point to block header */
    for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
        if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
            break;              /* freed block at start or end of arena */

    if (bp + bp->s.size == p->s.ptr) {  /* join to upper nbr */
        bp->s.size += p->s.ptr->s.size;
        bp->s.ptr = p->s.ptr->s.ptr;
    } else {
        bp->s.ptr = p->s.ptr;
    }

    if (p + p->s.size == bp) {  /* join to lower nbr */
        p->s.size += bp->s.size;
        p->s.ptr = bp->s.ptr;
    } else {
        p->s.ptr = bp;
    }

    freep = p;
}
