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

#if defined (CONFIG_MDOMAINS) || defined(CONFIG_MUNITS)
class cpu_context_t
{
    public:
    inline void operator= (cpu_context_t a) { raw = a.raw; }
    inline bool operator== (const cpu_context_t rhs) const { return (raw == rhs.raw); }
    inline bool operator!= (const cpu_context_t rhs) const { return (raw != rhs.raw); }

    cpu_context_t root_context(void)
    {
        cpu_context_t root;
        root.domain = domain;
        root.unit = 0xFFFF;
        return root;
    }

    union {
        struct {
            execution_unit_t unit;
            scheduler_domain_t domain;
        };
        word_t raw;
    };
} ;
#else
typedef word_t cpu_context_t;
#endif

#endif /* ! defined(ASSEMBLY) */

#endif /* ! __TYPES_H__ */
