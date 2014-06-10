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

#if defined(CONFIG_ARM_V5) || defined(CONFIG_ARM_V6) || (CONFIG_ARM_VER >= 6)

CONST INLINE
word_t msb(word_t w)
{
    word_t zeros;
#if defined(_lint)
    zeros = 0;
#endif
    __asm__ (
        "   clz     "_(zeros)", "_(w)"  "
#if defined(__GNUC__)
        : [zeros] "=r" (zeros)
        : [w] "r" (w)
#endif
    );
    return 31-zeros;
}

#else

#include <kernel/generic/lib.h>

CONST INLINE
word_t msb(word_t w)
{
    return msb_binary(w);
}

#endif

#endif
