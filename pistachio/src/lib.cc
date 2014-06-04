/*
 * Description:   Generic helper functions
 */
#include <l4.h>
#include <debug.h>

/*
 * Declare as weak to allow overloading by optimized processor
 * specific versions.
 */

extern "C" WEAK void * memcpy (void * dst, const void * src, unsigned int len)
{
    u8_t *d = (u8_t *) dst;
    u8_t *s = (u8_t *) src;

    while (len-- > 0)
        *d++ = *s++;

    return dst;
}

extern "C" WEAK void * memset (void * dst, unsigned int c, unsigned int len)
{
    u8_t *s = (u8_t *) dst;

    while (len-- > 0)
        *s++ = c;

    return dst;
}

extern "C" WEAK void strcpy(char * dst, const char * src)
{
    while(*src)
        *dst++ = *src++;
    *dst = '\0';
}

