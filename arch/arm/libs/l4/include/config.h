#ifndef __L4__ARM__CONFIG_H__
#define __L4__ARM__CONFIG_H__

/*
 * Under v4 and v5 the UTCB is managed by the kernel, so mark this by
 * setting NO_UTCB_RELOCATE.
 *
 * This file in included by L4 user code and Linux, which have
 * different mechanisms for identifying the ARM arch, thus the
 * repeated tests.
 */
#if defined(__ARMv__) 
#define __ARCH_VER __ARMv__
#elif defined(__LINUX_ARM_ARCH__)
#define __ARCH_VER __LINUX_ARM_ARCH__
#else
#error "ARMv or LINUX_ARM_ARCH not defined"
#endif

/**
 * Size of a UTCB in bytes 
 */
#define UTCB_BITS      (8)
#define UTCB_SIZE      ((word_t)1 << UTCB_BITS)
#define UTCB_ALIGNMENT 0x100

#include <l4/arch/ver/page.h>

/* FIXME: Check the correct values for USER_HW_PAGE_PERMS */
#if (__ARCH_VER == 5) || (__ARCH_VER == 4)

#define NO_UTCB_RELOCATE

#elif (__ARCH_VER == 6)

#else
#error "Unknown ARM version"
#endif

#endif /* !__L4_ARM__CONFIG_H__ */
