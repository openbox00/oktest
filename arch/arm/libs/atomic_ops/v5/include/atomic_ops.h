/*
 * This file is only included by the main atomic_ops.h, so all of that
 * file's definitions are available.
 */

#ifndef __ARCH_ATOMIC_OPS_H__
#define __ARCH_ATOMIC_OPS_H__

#if defined(__ATOMIC_OPS_IN_KERNEL__) && !defined(MACHINE_SMP)

#if CONFIG_NUM_UNITS > 1
#error CONFIG_NUM_UNITS > 1 but smp not defined in machines.py.
#endif 
#include <atomic_ops/unsafe_generic.h>

#else

static okl4_atomic_plain_word_t okl4_atomic_set(okl4_atomic_word_t* target,
                                      okl4_atomic_plain_word_t value);

/* Bitwise operations. */
void okl4_atomic_and(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask);
okl4_atomic_plain_word_t okl4_atomic_and_return(okl4_atomic_word_t* target,
                                       okl4_atomic_plain_word_t mask);

void okl4_atomic_or(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask);
okl4_atomic_plain_word_t okl4_atomic_or_return(okl4_atomic_word_t* target,
                                       okl4_atomic_plain_word_t mask);

void okl4_atomic_xor(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask);
okl4_atomic_plain_word_t okl4_atomic_xor_return(okl4_atomic_word_t* target,
                                       okl4_atomic_plain_word_t mask);

/* Bitfield operations. */
static void okl4_atomic_set_bit(okl4_atomic_word_t *target, unsigned long bit);
static void okl4_atomic_clear_bit(okl4_atomic_word_t *target, unsigned long bit);
static void okl4_atomic_change_bit(okl4_atomic_word_t *target, unsigned long bit);

/* Arithmetic operations. */
void okl4_atomic_add(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v);
okl4_atomic_plain_word_t okl4_atomic_add_return(okl4_atomic_word_t *target,
                                          okl4_atomic_plain_word_t v);

void okl4_atomic_sub(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v);
okl4_atomic_plain_word_t okl4_atomic_sub_return(okl4_atomic_word_t *target,
                                          okl4_atomic_plain_word_t v);

static void okl4_atomic_inc(okl4_atomic_word_t *target);
static okl4_atomic_plain_word_t okl4_atomic_inc_return(okl4_atomic_word_t *target);

static void okl4_atomic_dec(okl4_atomic_word_t *target);
static okl4_atomic_plain_word_t okl4_atomic_dec_return(okl4_atomic_word_t *target);

/* General operations. */
int okl4_atomic_compare_and_set(okl4_atomic_word_t *target, okl4_atomic_plain_word_t old_val,
                                                 okl4_atomic_plain_word_t new_val);

/* Memory barrier operations. */
static void okl4_atomic_barrier_write(void);
static void okl4_atomic_barrier_write_smp(void);
static void okl4_atomic_barrier_read(void);
static void okl4_atomic_barrier_read_smp(void);
static void okl4_atomic_barrier(void);
static void okl4_atomic_barrier_smp(void);

/* Inline functions */

static inline okl4_atomic_plain_word_t
okl4_atomic_set(okl4_atomic_word_t* target, okl4_atomic_plain_word_t value)
{
    target->value = value;
    return value;
}

static inline void
okl4_atomic_inc(okl4_atomic_word_t *target)
{
    okl4_atomic_add(target, 1);
}

static inline void
okl4_atomic_dec(okl4_atomic_word_t *target)
{
    okl4_atomic_add(target, -1);
}

static inline okl4_atomic_plain_word_t
okl4_atomic_inc_return(okl4_atomic_word_t *target)
{
    return okl4_atomic_add_return(target, 1);
}

static inline okl4_atomic_plain_word_t
okl4_atomic_dec_return(okl4_atomic_word_t *target)
{
    return okl4_atomic_add_return(target, -1);
}

static inline void
okl4_atomic_set_bit(okl4_atomic_word_t* target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_plain_word_t) * 8);
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_plain_word_t) * 8));

    okl4_atomic_or(&target[word], mask);
}

void __okl4_atomic_asm_bit_clear(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask);

static inline void
okl4_atomic_clear_bit(okl4_atomic_word_t* target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_plain_word_t) * 8);
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_plain_word_t) * 8));

    __okl4_atomic_asm_bit_clear(&target[word], mask);
}

static inline void
okl4_atomic_change_bit(okl4_atomic_word_t* target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_plain_word_t) * 8);
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_plain_word_t) * 8));

    okl4_atomic_xor(&target[word], mask);
}

/* TODO: Not implemented! */
static inline void
okl4_atomic_barrier_write(void)
{
}

static inline void
okl4_atomic_barrier_write_smp(void)
{
}

static inline void
okl4_atomic_barrier_read(void)
{
}

static inline void
okl4_atomic_barrier_read_smp(void)
{
}

static inline void
okl4_atomic_barrier(void)
{
}

static inline void
okl4_atomic_barrier_smp(void)
{
}

#endif /* defined(__ATOMIC_OPS_IN_KERNEL__) && !defined(MACHINE_SMP) */

#endif /* _ARCH_ATOMIC_OPS_H__ */
