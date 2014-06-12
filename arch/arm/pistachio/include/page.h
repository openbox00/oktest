/*
 * Description:   ARM specific MM
 */

#ifndef __ARCH__ARM__PAGE_H__
#define __ARCH__ARM__PAGE_H__

/*
 * Supported Page Sizes:
 * - 1k ("tiny" pages) -- ARMv5 *broken*
 * - 4k
 * - 64k
 * - 1M ("sections")
 * - 16M ("supersections")  -- ARMv6
 */
#define ARM_SECTION_BITS        12
#define ARM_SECTION_SIZE        (1UL << (32 - ARM_SECTION_BITS))
#define ARM_NUM_SECTIONS        (1UL << ARM_SECTION_BITS)

/* 256/256 byte levels */
#define ARM_TOP_LEVEL_BITS      26

 #define ARM_PAGE_SIZE          PAGE_SIZE_4K
 #define ARM_PAGE_BITS          PAGE_BITS_4K
 #define TINY_PAGE
 #define HW_PGSHIFTS            { 12, 16, 20, ARM_TOP_LEVEL_BITS, 32 }

#define HW_VALID_PGSIZES (      \
                TINY_PAGE       \
                (1<<12) |       \
                (1<<16) |       \
                (1<<20))

#define PAGE_SIZE_TOP_LEVEL     (1UL << ARM_TOP_LEVEL_BITS)
#define PAGE_BITS_TOP_LEVEL     (ARM_TOP_LEVEL_BITS)
#define PAGE_MASK_TOP_LEVEL     (~(PAGE_SIZE_TOP_LEVEL - 1))


#define PAGE_SIZE_1K            (1UL << 10)
#define PAGE_SIZE_4K            (1UL << 12)
#define PAGE_SIZE_64K           (1UL << 16)
#define PAGE_SIZE_1M            (1UL << 20)
#define PAGE_SIZE_16M           (1UL << 24)

#define PAGE_BITS_1K            (10)
#define PAGE_BITS_4K            (12)
#define PAGE_BITS_64K           (16)
#define PAGE_BITS_1M            (20)
#define PAGE_BITS_16M           (24)

#define PAGE_MASK_1K            (~(PAGE_SIZE_1K - 1))
#define PAGE_MASK_4K            (~(PAGE_SIZE_4K - 1))
#define PAGE_MASK_64K           (~(PAGE_SIZE_64K -1))
#define PAGE_MASK_1M            (~(PAGE_SIZE_1M - 1))
#define PAGE_MASK_16M           (~(PAGE_SIZE_16M- 1))
#define PID_MASK        0x7f

#endif /* __ARCH__ARM__PAGE_H__ */
