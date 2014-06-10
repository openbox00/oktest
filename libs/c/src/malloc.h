#ifndef _LIBS_MALLOC_H_
#define _LIBS_MALLOC_H_

extern Header __malloc_base;                /* empty list to get started */
extern Header *_kr_malloc_freep;            /* start of free list */

#define freep   _kr_malloc_freep
#define base    __malloc_base

#endif /* _LIBS_MALLOC_H_ */

