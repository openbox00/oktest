/*
 * Description:   Shared headers needed by most source files
 */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <kernel/arch/special.h> /* for msb */
#include <atomic_ops/atomic_ops.h>

//#define TRACEBM TRACEF
#define TRACEBM(...)

#define BITMAP_SIZE(bits)   (((bits) + (BITS_WORD-1)) / BITS_WORD)

#define BITMAP_MASK(bit)    (1UL << ((bit) % BITS_WORD))
#define BITMAP_BITNO(bit)   (bit)

/* base bitmap is just a machine word in size */
typedef word_t bitmap_t;

INLINE void bitmap_init(bitmap_t * bm, word_t length, word_t set);
INLINE void bitmap_set(bitmap_t * bm, word_t bit);
INLINE void bitmap_clear(bitmap_t * bm, word_t bit);
INLINE int bitmap_isset(bitmap_t * bm, word_t bit);
INLINE int bitmap_isallset(bitmap_t * bm, word_t length);
INLINE int bitmap_isallclear(bitmap_t * bm, word_t length);
INLINE int bitmap_israngeset(bitmap_t * bm, word_t first, word_t last);
INLINE int bitmap_findfirstset(bitmap_t * bm, word_t length);
INLINE int bitmap_findfirstclear(bitmap_t * bm, word_t length);


/* initialize the bitmap to all set or all clear  (length in bits) */
INLINE void bitmap_init(bitmap_t * bm, word_t length, word_t set)
{
    word_t i;

    TRACEBM("bitmap_init bm = %p, length = %d, set = %d\n", bm, length, set);
    /* normalize set value to 0 or -1 */
    if (set) {
        set = ~0UL;
    }

    /* initialize all words in bitmap */
    for (i = 0; i < BITMAP_SIZE(length); i++)
        bm[i] = set;

    /* correctly clear unused bits in last in bitmap word */
    if (length % BITS_WORD) {
        bm[i-1] &= ((1UL << ((length % BITS_WORD))) - 1);
    }

    TRACEBM("bitmap_init: last word %lx\n", bm[i-1]);
}

/* set a particular bit in a bitmap
 * threadsafe on MUNITS
 */
INLINE void bitmap_set(bitmap_t * bm, word_t bit)
{
    TRACEBM("bm=%p, bit=%ld\n", bm, bit);
#ifdef CONFIG_MUNITS
    okl4_atomic_word_t * w = (okl4_atomic_word_t *)&bm[bit/BITS_WORD];
    okl4_atomic_plain_word_t mask = BITMAP_MASK(bit);
    okl4_atomic_or(w, mask);
#else
    bm[bit/BITS_WORD] |= BITMAP_MASK(bit);
#endif
}

/* clear a particular bit in the bitmap
 * threadsafe on MUNITS
 */
INLINE void bitmap_clear(bitmap_t * bm, word_t bit)
{
    TRACEBM("bm=%p, bit=%ld\n", bm, bit);
#ifdef CONFIG_MUNITS
    okl4_atomic_word_t * w = (okl4_atomic_word_t *)&bm[bit/BITS_WORD];
    okl4_atomic_plain_word_t mask = ~BITMAP_MASK(bit);
    okl4_atomic_and(w, mask);
#else
    bm[bit/BITS_WORD] &= ~BITMAP_MASK(bit);
#endif
}


/* determine the status of a particular bit in the bitmap */
INLINE int bitmap_isset(bitmap_t * bm, word_t bit)
{
    TRACEBM("bm=%p, bit=%ld\n", bm, bit);
    return (bm[bit/BITS_WORD]) & BITMAP_MASK(bit);
}

/* Check if every bit in the bitmap is set (length in bits) */
INLINE int bitmap_isallset(bitmap_t * bm, word_t length)
{
    word_t i;
    word_t check = ~0UL;

    TRACEBM("bitmap_isallset bm = %p, length = %d\n", bm, length);

    /* check complete words in bitmap */
    for (i = 0; i < BITMAP_SIZE(length)-1; i++)
        if(bm[i] != check)
            return 0;

    /* correctly check last partial word in bitmap */
    return ( bm[i] == ((1UL << ((length % BITS_WORD))) - 1));
}

/* Check if every bit in the bitmap is clear (length in bits) */
INLINE int bitmap_isallclear(bitmap_t * bm, word_t length)
{
    word_t i;
    word_t check = 0;

    TRACEBM("bitmap_isallclear bm = %p, length = %d\n", bm, length);

    /* check complete words in bitmap */
    for (i = 0; i < BITMAP_SIZE(length)-1; i++)
        if(bm[i] != check)
            return 0;

    /* correctly check last partial word in bitmap */
    return ( bm[i] == 0 );
}

/**
 * Check if every bit in range of the bitmap is set
 * *inclusive* of first and last
 */
INLINE int bitmap_israngeset(bitmap_t * bm, word_t first, word_t last)
{
    word_t i = first / BITS_WORD;
    word_t j = last / BITS_WORD;
    word_t first_mask = ((1<<(first % BITS_WORD))-1);   /* exclusive mask */
    word_t last_mask = ((1<<((last % BITS_WORD)+1))-1); /* inclusive mask */
    word_t check;

    TRACEBM("bitmap_israngeset bm = %p, a = %ld, b = %ld\n", bm, first, last);

    if (EXPECT_TRUE(i == j))
    {
        check = first_mask ^ last_mask;
        if ((bm[i] & check) == check)
            return 1;
        return 0;
    }

    check = ~first_mask;
    do {
        if ((bm[i] & check) != check)
            return 0;
        check = TWOSCOMP(1UL);
        i++;
    } while (i < j);

    check = last_mask;
    if ((bm[i] & check) != check)
        return 0;

    return 1;
}


/*
 * Find the first bit set in the bitmap (length in bits)
 * returns -1 if no bit is found.
 *
 * length must be same as length used for bitmap_init()
 */
INLINE int bitmap_findfirstset(bitmap_t * bm, word_t length)
{
    word_t i;

    /* scan the bitfield for a set bit */
    for (i = 0; i < BITMAP_SIZE(length); i++)
    {
        if (EXPECT_TRUE(bm[i] != 0)) {
            TRACEBM("bm[i] = %lx, msb = %ld\n", bm[i], msb(bm[i]));
            return (i*BITS_WORD) + BITMAP_BITNO( msb(bm[i]) );
        }
    }

    TRACEBM("No bits set\n");
    return -1;
}

/*
 * Find the first bit clear in the bitmap (length in bits)
 * returns -1 if no bit is found.
 *
 * length must be same as length used for bitmap_init()
 */
INLINE int bitmap_findfirstclear(bitmap_t * bm, word_t length)
{
    word_t i;

    /* scan the bitfield for a set bit */
    for (i = 0; i < BITMAP_SIZE(length); i++)
    {
        word_t bits = !bm[i];
        if (bits) {
            TRACEBM("bm[i] = %lx, msb = %ld\n", bm[i], msb(bits));
            return (i*BITS_WORD) + BITMAP_BITNO( msb(bits) );
        }
    }

    TRACEBM("No bits clear\n");
    return -1;
}

#endif /* __BITMAP_H__ */
