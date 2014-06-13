#ifndef __TYPES_H__
#define __TYPES_H__

#include <kernel/types.h>
#include <config.h>

#if !defined(ASSEMBLY)


typedef u16_t execution_unit_t;
typedef u16_t scheduler_domain_t;
typedef word_t cpu_context_t;
#endif /* ! defined(ASSEMBLY) */

#endif /* ! __TYPES_H__ */
