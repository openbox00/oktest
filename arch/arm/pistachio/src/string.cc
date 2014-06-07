/*
 * Description:    ARM optimised string operations
 */

#include <l4.h>
#include <debug.h>

extern "C" void * memcpy (void * dst, const void * src, unsigned int len)
{
    if (EXPECT_FALSE( ((word_t)dst | (word_t)src | len) & 3 ))
    {
        u8_t *d = (u8_t *) dst;
        u8_t *s = (u8_t *) src;

        while (len-- > 0)
            *d++ = *s++;
    } else {
        u32_t * RESTRICT d = (u32_t *) dst;
        u32_t * RESTRICT s = (u32_t *) src;

        len = len / 4;

        while (len > 4)
        {
            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
            d[3] = s[3];
            len -= 4;
            d += 4;
            s += 4;
        }

        while (len-- > 0)
            *d++ = *s++;
    }
    return dst;
}

extern "C" void * memset (void * dst, unsigned int c, unsigned int len)
{
    u8_t *s = (u8_t *) dst;
    u8_t val = c;

    if (EXPECT_FALSE(((word_t)s & 3)))
    {
        while (((word_t)s & 3) && len-- > 0) {
            *s++ = val;
        };
    }

    u32_t *sw = (u32_t*)s;

    u32_t cw = (val | val << 8);
    cw = cw | (cw << 16);

    while (len >= 16) {
        *sw++ = cw;
        *sw++ = cw;
        *sw++ = cw;
        *sw++ = cw;
        len -= 16;
    }
    if (len >= 8) {
        *sw++ = cw;
        *sw++ = cw;
        len -= 8;
    }
    if (len >= 4) {
        *sw++ = cw;
        len -= 4;
    }

    s = (u8_t*)sw;

    while (len-- > 0)
        *s++ = val;

    return dst;
}

