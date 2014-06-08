#ifndef _LIBC_K_R_MALLOC_H_
#define _LIBC_K_R_MALLOC_H_

typedef long long Align;        /* for alignment to long long boundary */

union header {                  /* block header */
    struct {
        union header *ptr;      /* next block if on free list */
        unsigned size;          /* size of this block */
    } s;
    Align x;                    /* force alignment of blocks */
};

typedef union header Header;

Header *morecore(unsigned num_units);

#endif /* _LIBC_K_R_MALLOC_H_ */
