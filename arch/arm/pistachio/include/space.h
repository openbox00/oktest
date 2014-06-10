/*
 * Description: ARM specific space implementation.
 */

#ifndef __GLUE__V4_ARM__SPACE_H
#define __GLUE__V4_ARM__SPACE_H

#include <clist.h>


/* ARM provides its own implementation of space->arch_free */
#define HAVE_ARCH_FREE_SPACE

/* Use the longest possible bitmap without
 * increasing size of space data structure past
 * 128 bytes
 */
#define UTCB_BITMAP_LENGTH ((92 - 24)*8)


/* Include ARM version specfic space definition and implementations */
#include <arch/ver/space.h>

INLINE pgent_t *generic_space_t::pgent(word_t num, word_t cpu)
{
    return &(pdir[num]);
}

INLINE bool generic_space_t::is_user_area (addr_t addr)
{
    return ((word_t)addr < USER_AREA_END);
}

/* May as well return kernel space since mappings should be identical for
 * the kernel area as in the each of the user's address spaces.
 */
space_t * get_kernel_space() PURE;

INLINE space_t * get_kernel_space()
{
    return get_arm_globals()->kernel_space;
}

INLINE bool generic_space_t::does_tlbflush_pay(word_t log2size)
{
    return log2size > 12;
}

#endif
