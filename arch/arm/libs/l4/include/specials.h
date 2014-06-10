/*
 * Description:   ARM specific functions and defines
 */
#ifndef __L4__ARM__SPECIALS_H__
#define __L4__ARM__SPECIALS_H__

#include <compat/c.h>

/*
 * Architecture specific helper functions.
 */

L4_INLINE int __L4_Msb(L4_Word_t w) CONST;

L4_INLINE int
__L4_Msb(L4_Word_t w)
{
    int bitnum;

    if (w == 0)
        return 32;

    for (bitnum = 0, w >>= 1; w != 0; bitnum++)
        w >>= 1;

    return bitnum;
}

#define ARCH_THREAD_INIT

#endif /* !__L4__ARM__SPECIALS_H__ */
