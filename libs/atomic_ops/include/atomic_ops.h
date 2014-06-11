#ifndef __ATOMIC_OPS_H__
#define __ATOMIC_OPS_H__

/*
 * Kernel and Userland definitions.
 *
 * The atomic ops API is common between the kernel and userland
 * but many of the implementation details differ.  
 */
#ifdef KENGE_PISTACHIO

#define __ATOMIC_OPS_IN_KERNEL__
#include <kernel/config.h>
#include <kernel/types.h>

typedef word_t okl4_atomic_plain_word_t;

#else /* In userland */

#include <l4/types.h>
typedef L4_Word_t okl4_atomic_plain_word_t;

#endif /* kernel or userland */

typedef struct {
    volatile okl4_atomic_plain_word_t value;
} okl4_atomic_word_t;

#define ATOMIC_INIT(i)  { (i) }

static inline void
okl4_atomic_init(okl4_atomic_word_t *a, okl4_atomic_plain_word_t v)
{
    a->value = v;
}


#endif /* __ATOMIC_OPS_H__ */
