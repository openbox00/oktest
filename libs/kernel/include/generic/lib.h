/*
 * Description:   declaration of lib functions
 */
#ifndef __GENERIC__LIB_H__
#define __GENERIC__LIB_H__

C_LINKAGE void * memcpy (void * dst, const void * src, unsigned int len);
C_LINKAGE void * memset (void * dst, unsigned int c, unsigned int len);
#if defined(__cplusplus)
C_LINKAGE WEAK void strcpy(char * dst, const char * src);
#endif

INLINE word_t msb_simple(word_t w);
INLINE word_t msb_byte_search(word_t w);
INLINE word_t msb_binary(word_t w);
INLINE word_t msb_binary2(word_t w);
INLINE word_t msb_nobranch(word_t w);


INLINE word_t msb_simple(word_t w)
{
    word_t bit = BITS_WORD - 1;
    word_t test = 1UL << (BITS_WORD-1);

    while (!(w & test)) {
        w <<= 1;
        bit --;
    }
    return bit;
}

INLINE word_t msb_byte_search(word_t w)
{
    word_t bit = BITS_WORD - 1;
    word_t test = 1UL << bit;

    while (!(w >> (BITS_WORD-8))) {
        w <<= 8;
        bit -= 8;
    }

    while (!(w & test)) {
        w <<= 1;
        bit --;
    }
    return bit;
}

INLINE word_t msb_binary(word_t w)
{
    word_t bit = BITS_WORD - 1;

#if defined(L4_64BIT)
    if (!(w >> (BITS_WORD-32))) {
        bit -= 32;
        w <<= 32;
    }
#endif

    if (!(w >> (BITS_WORD-16))) {
        bit -= 16;
        w <<= 16;
    }

    if (!(w >> (BITS_WORD-8))) {
        bit -= 8;
        w <<= 8;
    }

    if (!(w >> (BITS_WORD-4))) {
        bit -= 4;
        w <<= 4;
    }

    if (!(w >> (BITS_WORD-2))) {
        bit -= 2;
        w <<= 2;
    }

    if (!(w >> (BITS_WORD-1))) {
        bit -= 1;
    }

    return bit;
}

/* Seems to be fast on i386 cpus */
INLINE word_t msb_binary2(word_t w)
{
    word_t bit=0;

#if defined(L4_64BIT)
    if (w & 0xffffffff00000000ULL) {
        bit |= 32;
        w >>= 32;
    }
#endif

    if (w & 0xffff0000UL) {
        bit |= 16;
        w >>= 16;
    }

    if (w & 0xff00) {
        bit |= 8;
        w >>= 8;
    }

    if (w & 0xf0) {
        bit |= 4;
        w >>= 4;
    }

    if (w & 0xc) {
        bit |= 2;
        w >>= 2;
    }

    if (w & 0x2) {
        bit |= 1;
    }

    return bit;
}

INLINE word_t msb_nobranch(word_t w)
{
    word_t test;
    word_t bit = 0;

#if defined(L4_64BIT)
    test = ((w & (((1ULL << 32) - 1)<<32)) != 0) * 32; w >>= test; bit |= test;
#endif
    test = ((w & (((1UL << 16) - 1)<<16)) != 0 ? 1 : 0) * 16; w >>= test; bit |= test;
    test = ((w & (((1UL <<  8) - 1)<< 8)) != 0 ? 1 : 0) *  8; w >>= test; bit |= test;
    test = ((w & (((1UL <<  4) - 1)<< 4)) != 0 ? 1 : 0) *  4; w >>= test; bit |= test;
    test = ((w & (((1UL <<  2) - 1)<< 2)) != 0 ? 1 : 0) *  2; w >>= test; bit |= test;
    test = ((w & (((1UL <<  1) - 1)<< 1)) != 0 ? 1 : 0) *  1; w >>= test; bit |= test;

    return bit;
}

INLINE word_t max(word_t a, word_t b)
{
    return (a > b) ? a : b;
}

INLINE word_t min(word_t a, word_t b)
{
    return (a > b) ? b : a;
}

#endif /* !__GENERIC__LIB_H__ */
