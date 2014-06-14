/*
 * Description:   ARM-specific special optimisations
 */
#ifndef __ARCH__ARM__SPECIAL_H__
#define __ARCH__ARM__SPECIAL_H__

#include <kernel/arch/asm.h>
/*
 * Find Most Significant bit.
 * NB - check for w == 0 must be done before calling msb()
 * */
CONST INLINE
word_t msb(word_t w)
{
    word_t zeros;
    __asm__ (
        "   clz     "_(zeros)", "_(w)"  "
        : [zeros] "=r" (zeros)
        : [w] "r" (w)
    );
    return 31-zeros;
}
#endif
