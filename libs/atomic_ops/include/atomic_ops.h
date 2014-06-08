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

#if !defined(NANOKERNEL)
#include <l4/types.h>
typedef L4_Word_t okl4_atomic_plain_word_t;
#else
#include <okl4/types.h>
typedef okl4_word_t okl4_atomic_plain_word_t;
#endif

#endif /* kernel or userland */

/*-------------------------------------------------------------------------*/
                        /* Atomic Ops API. */

/*
 * IMPORTANT!
 * If you plan to change the structure okl4_atomic_word_t, please add the new
 * elements after value. For more information read the comment in
 * arch/arm/libs/atomic_ops/v5/src/arm_atomic_ops.spp:66
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    volatile okl4_atomic_plain_word_t value;
} okl4_atomic_word_t;

#define ATOMIC_INIT(i)  { (i) }

static inline void
okl4_atomic_init(okl4_atomic_word_t *a, okl4_atomic_plain_word_t v)
{
    a->value = v;
}

#if defined(ARCH_ARM) && defined(ARCH_VER) && (ARCH_VER < 6) && \
         (!defined(__ATOMIC_OPS_IN_KERNEL__) || defined(MACHINE_SMP))

/* 
 * If it is ARMv4/v5, the function declarations may change
 * and are defined in the arch specific header file,
 * as some of then can't be declared static because of
 * the assembler implementation.
 */

#else 

/** Atomic set and return new value */
static okl4_atomic_plain_word_t okl4_atomic_set(okl4_atomic_word_t* target,
                                      okl4_atomic_plain_word_t value);

/*** Bitwise operations. ***/

/** Atomic AND */
static void okl4_atomic_and(okl4_atomic_word_t* target,
                       okl4_atomic_plain_word_t mask);
/** Atomic AND and return new value */
static okl4_atomic_plain_word_t okl4_atomic_and_return(okl4_atomic_word_t* target,
                                             okl4_atomic_plain_word_t mask);

/** Atomic AND and return old value */
static okl4_atomic_plain_word_t okl4_atomic_and_return_old(okl4_atomic_word_t* target,
                                             okl4_atomic_plain_word_t mask);
/** Atomic OR */
static void okl4_atomic_or(okl4_atomic_word_t* target,
                      okl4_atomic_plain_word_t mask);
/** Atomic OR and return new value */
static okl4_atomic_plain_word_t okl4_atomic_or_return(okl4_atomic_word_t* target,
                                            okl4_atomic_plain_word_t mask);

/** Atomic XOR */
static void okl4_atomic_xor(okl4_atomic_word_t* target,
                       okl4_atomic_plain_word_t mask);
/** Atomic XOR and return new value */
static okl4_atomic_plain_word_t okl4_atomic_xor_return(okl4_atomic_word_t* target,
                                             okl4_atomic_plain_word_t mask);

/*** Bitfield operations. ***/

static void okl4_atomic_set_bit(okl4_atomic_word_t *target, unsigned long bit);
static void okl4_atomic_clear_bit(okl4_atomic_word_t *target, unsigned long bit);
static int okl4_atomic_clear_bit_return_old(okl4_atomic_word_t *target, unsigned long bit);
static void okl4_atomic_change_bit(okl4_atomic_word_t *target, unsigned long bit);

/*** Arithmetic operations. ***/
static void okl4_atomic_add(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v);
static okl4_atomic_plain_word_t okl4_atomic_add_return(okl4_atomic_word_t *target,
                                             okl4_atomic_plain_word_t v);

static void okl4_atomic_sub(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v);
static okl4_atomic_plain_word_t okl4_atomic_sub_return(okl4_atomic_word_t *target,
                                             okl4_atomic_plain_word_t v);

static void okl4_atomic_inc(okl4_atomic_word_t *target);
static okl4_atomic_plain_word_t okl4_atomic_inc_return(okl4_atomic_word_t *target);

static void okl4_atomic_dec(okl4_atomic_word_t *target);
static okl4_atomic_plain_word_t okl4_atomic_dec_return(okl4_atomic_word_t *target);

/*** General operations. ***/

static int okl4_atomic_compare_and_set(okl4_atomic_word_t *target,
                                  okl4_atomic_plain_word_t old_val,
                                  okl4_atomic_plain_word_t new_val);

/* Memory barrier operations. */

static void okl4_atomic_barrier_write(void);
static void okl4_atomic_barrier_write_smp(void);
static void okl4_atomic_barrier_read(void);
static void okl4_atomic_barrier_read_smp(void);
static void okl4_atomic_barrier(void);
static void okl4_atomic_barrier_smp(void);

#endif

/*---------------------------------------------------------------------------*/

/* Architecture independent definitions. */

static okl4_atomic_plain_word_t okl4_atomic_read(okl4_atomic_word_t *target);
static void okl4_atomic_compiler_barrier(void);

static inline okl4_atomic_plain_word_t
okl4_atomic_read(okl4_atomic_word_t *target)
{
    return target->value;
}

static inline void okl4_atomic_compiler_barrier(void)
{
#if defined(__RVCT__) || defined(__RVCT_GNU__)
    __memory_changed();
#elif defined(__ADS__)
    __asm("");
#else
    asm volatile (""::: "memory");
#endif
}


/* Architecture dependent definitions. */
#include <atomic_ops/arch/atomic_ops.h>

#ifdef __cplusplus
}
#endif

#endif /* __ATOMIC_OPS_H__ */
