#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../k_r_malloc.h"

#define NALLOC 100
uintptr_t __malloc_bss;
uintptr_t __malloc_top;

Header *_kr_malloc_freep = NULL;

void __malloc_init(uintptr_t heap_base, uintptr_t heap_end);

void
__malloc_init(uintptr_t heap_base, uintptr_t heap_end)
{
    __malloc_bss = heap_base;
    __malloc_top = heap_end + 1;

}

#define round_up(address, size) ((((address) + (size-1)) & (~(size-1))))

/*
 * sbrk equiv
 */
Header *
morecore(unsigned nu)
{
    uintptr_t nb;
    uintptr_t cp;
    Header *up;

    cp = __malloc_bss;

    nb = round_up(nu * sizeof(Header), NALLOC);

    if (__malloc_bss + nb > __malloc_top) {
        return NULL;
    }
    __malloc_bss += nb;
    up = (Header *)cp;
    up->s.size = nb / sizeof(Header);
    free((void *)(up + 1));
    return _kr_malloc_freep;
}
