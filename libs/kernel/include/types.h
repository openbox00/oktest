/*
 * Description: Defines some basic, global types.
 */

#ifndef __KERNEL_TYPES_H__
#define __KERNEL_TYPES_H__

#if !defined(ASSEMBLY)

#include <kernel/arch/types.h>
/* At this point we should have word_t defined */
#include <l4/types.h>

/**
 *      addr_t - data type to store addresses
 */
typedef void*                   addr_t;

/**
 * Add offset to address.
 * @param addr          original address
 * @param off           offset to add
 * @return new address
 */
INLINE addr_t addr_offset(addr_t addr, word_t off)
{
    return (addr_t)((word_t)addr + off);
}

/**
 * Apply mask to an address.
 * @param addr          original address
 * @param mask          address mask
 * @return new address
 */
INLINE addr_t addr_mask (addr_t addr, word_t mask)
{
    return (addr_t) ((word_t) addr & mask);
}

/**
 * Align address upwards.  It is assumed that the alignment is a power of 2.
 * @param addr          original address
 * @param align         alignment
 * @return new address
 */
INLINE addr_t addr_align_up (addr_t addr, word_t align)
{
    return addr_mask (addr_offset (addr, align - 1), ~(align - 1));
}

#define INVALID_ADDR ((addr_t)~0UL)

#ifndef NULL
#define NULL 0
#endif

/* Scheduling-related types */
typedef int prio_t;

// Mark as a data-structure alias - AGW
#define BITS_WORD WORD_T_BIT

typedef void (*continuation_t)(void);
#define CONTINUATION_FUNCTION(name) NORETURN void name (void)

#if defined(_lint)
void __asm_continuation(void);
#define ASM_CONTINUATION __asm_continuation
#else
#define ASM_CONTINUATION (continuation_t) __return_address()
#endif

/* System callback functions that need to activate the given
 * continuation when they complete. */
typedef void (*callback_func_t)(continuation_t);

typedef u16_t execution_unit_t;
typedef u16_t scheduler_domain_t;

#endif /* !defined(ASSEMBLY) */

#endif /* !__KERNEL_TYPES_H__ */
