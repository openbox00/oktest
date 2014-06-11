/**
 * @file
 * @brief Types unique to the core OKL4 kernel.
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <kernel/types.h>
#include <config.h>

#if !defined(ASSEMBLY)
/**
 * Add offset to address.
 * @param addr          original address
 * @param off           offset to add
 * @return new address
 */
INLINE addr_t addr_offset(addr_t addr, addr_t off)
{
    return (addr_t)((word_t)addr + (word_t)off);
}


typedef u16_t execution_unit_t;
typedef u16_t scheduler_domain_t;
typedef word_t cpu_context_t;
#endif /* ! defined(ASSEMBLY) */

#endif /* ! __TYPES_H__ */
