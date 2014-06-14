/*
 * Description:   ARMv5 specific kernel config
 */

#ifndef __ARCH__ARM__V5__CONFIG_H__
#define __ARCH__ARM__V5__CONFIG_H__

/**
 * supported page sizes and access bits
 */
#define ARCH_PAGE_PERMS         2
#define ARCH_PAGE_SIZES         HW_VALID_PGSIZES

/* UTCB is kernel allocated */
#define CONFIG_UTCB_SIZE        0

/* Address space layout info */

#define USER_AREA_SECTIONS      ((ARM_NUM_SECTIONS * 14) / 16)

/* The UTCB area contains n (currently 256) 1MB sections, where each section
 * can be assigned to one address space. This limits the number of address
 * spaces to 256.
 * Once a section is assigned to an address space, the kernel allocates
 * UTCBs for that address space in that section.
 * Note: An entire section is used per address space so that the kernel
 * can use domain ids to enforce permissions.
 */
/* 0xe0000000 */
#define UTCB_AREA_SECTIONS_BITS 8
#define UTCB_AREA_SECTIONS      (1UL << UTCB_AREA_SECTIONS_BITS)
#define UTCB_AREA_START         USER_AREA_END
#define UTCB_AREA_BITS          (UTCB_AREA_SECTIONS_BITS + (32 - ARM_SECTION_BITS))
#define UTCB_AREA_SIZE          (UTCB_AREA_SECTIONS * ARM_SECTION_SIZE)
#define UTCB_AREA_END           (UTCB_AREA_START + UTCB_AREA_SIZE)

#define KERNEL_AREA_START       UTCB_AREA_END

#define ARCH_MAX_SPACES         256

#endif /*__ARCH__ARM__V5__CONFIG_H__*/
