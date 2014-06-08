/*
 * Description:   RVCT version of cache-flush-d
 */

#include <l4.h>
#include <arch/cache.h>

/*
 * XXX: These functions are expanded versions of the inline functions in
 * cache.h.  We need them because RVCT won't let us use encodings of mrc
 * that use r15, even though the register is not used by the instruction.
 */

__asm void arm_cache::cache_flush_d()
{
again
    mrc     p15, 0, r15, c7, c14, 3
    bne     again
    bx      lr
}
