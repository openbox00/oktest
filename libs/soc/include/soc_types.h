#ifndef __L4__PLATFORM_TYPES_H__
#define __L4__PLATFORM_TYPES_H__

#include <kernel/types.h>
#include <kernel/arch/types.h>
#include <kernel/macros.h>
#if !defined(ASSEMBLY)
#include <l4/arch/vregs.h>

/**
 * @brief The version of the SOC interface this header provides.
 */
#define SOC_API_VERSION 2


/*
 * The following classes need to be passed in through platform functions, but
 * the platform code should treat them as "opaque".
 */
typedef word_t tcb_h;
typedef word_t space_h;
typedef union {
    struct {
        u16_t unit;
        u16_t domain;
    } x;
    word_t  raw;
} cpu_context_h;

#if !defined(__cplusplus)
typedef word_t spaceid_t;
typedef word_t threadid_t;
typedef word_t acceptor_t;
typedef word_t notify_bits_t;
typedef word_t capid_t;
typedef UTCB_STRUCT utcb_t;
#else
class utcb_t;
#endif
#endif

#endif /* ! __L4__PLATFORM_TYPES_H__ */
