#include <string.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Fill memory at s with (n) * byte value 'c'
 */
#if !defined(CONFIG_SPEED)
void *
memset(void *s, int c, size_t n)
{
    unsigned char *ss = (unsigned char *)s; 
    while (n--) {
        *ss++ = (unsigned char) c;
    }
    return s;
}
#else
void *
memset(void *s, int c, size_t n)
{
    uintptr_t num, align, pattern, *p, x;
    unsigned char *mem = s;

    x = (unsigned char)c;
    align = sizeof(uintptr_t) - 1;

    if ((uintptr_t)s & align) {
        num = n > align ? (sizeof(uintptr_t) - ((uintptr_t)s & align)) : n;
        n -= num;
        while (num--)
            *mem++ = x;
    }

    num = (uintptr_t)n / sizeof(uintptr_t);

    p = (uintptr_t *)mem;

#if UINTPTR_MAX == UINT32_MAX   /*lint -e30 */
    pattern = x | x << 8 | x << 16 | x << 24;
#elif UINTPTR_MAX == UINT64_MAX /*lint -e30 */
    pattern = x | x << 8 | x << 16 | x << 24 |
        x << 32 | x << 40 | x << 48 | x << 56;
#else
#error UINTPTR_MAX not valid
#endif
    while (num) {
        /* simple hand unrolled loop */
        if ((num & 3) == 0) {
            num -= 4;
            *p++ = pattern;
            *p++ = pattern;
            *p++ = pattern;
            *p++ = pattern;
        } else {
            num--;
            *p++ = pattern;
        }
    }
    mem = (unsigned char *)p;

    num = n & align;
    while (num--)
        *mem++ = x;

    return s;
}
#endif
