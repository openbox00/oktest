/*
 * Description:   Shared headers needed by most source files
 */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <kernel/arch/special.h> /* for msb */
#include <atomic_ops/atomic_ops.h>

#define BITMAP_SIZE(bits)   (((bits) + (BITS_WORD-1)) / BITS_WORD)

#define BITMAP_MASK(bit)    (1UL << ((bit) % BITS_WORD))
#define BITMAP_BITNO(bit)   (bit)

/* base bitmap is just a machine word in size */
typedef word_t bitmap_t;

INLINE void bitmap_init(bitmap_t * bm, word_t length, word_t set);
INLINE void bitmap_set(bitmap_t * bm, word_t bit);
INLINE void bitmap_clear(bitmap_t * bm, word_t bit);
INLINE int bitmap_isset(bitmap_t * bm, word_t bit);
INLINE int bitmap_israngeset(bitmap_t * bm, word_t first, word_t last);


/* initialize the bitmap to all set or all clear  (length in bits) */
INLINE void bitmap_init(bitmap_t * bm, word_t length, word_t set)
{
    word_t i;
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
}

/* set a particular bit in a bitmap
 * threadsafe on MUNITS
 */
INLINE void bitmap_set(bitmap_t * bm, word_t bit)
{
    bm[bit/BITS_WORD] |= BITMAP_MASK(bit);
}

/* clear a particular bit in the bitmap
 * threadsafe on MUNITS
 */
INLINE void bitmap_clear(bitmap_t * bm, word_t bit)
{
    bm[bit/BITS_WORD] &= ~BITMAP_MASK(bit);
}


/* determine the status of a particular bit in the bitmap */
INLINE int bitmap_isset(bitmap_t * bm, word_t bit)
{
    return (bm[bit/BITS_WORD]) & BITMAP_MASK(bit);
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

INLINE int bitmap_findfirstset(bitmap_t * bm, word_t length)
{
    word_t i;
    for (i = 0; i < BITMAP_SIZE(length); i++)
    {
        if (EXPECT_TRUE(bm[i] != 0)) {
            return (i*BITS_WORD) + BITMAP_BITNO( msb(bm[i]) );
        }
    }
    return -1;
}


#endif /* __BITMAP_H__ */
