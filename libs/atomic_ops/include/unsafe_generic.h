
#ifndef __ARCH_ATOMIC_OPS_UNSAFE_GENERIC_H__
#define __ARCH_ATOMIC_OPS_UNSAFE_GENERIC_H__

static inline okl4_atomic_plain_word_t
okl4_atomic_set(okl4_atomic_word_t* target, okl4_atomic_plain_word_t value)
{
    target->value = value;
    return value;
}

/* Bitwise operations. */
static inline void
okl4_atomic_and(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    target->value &= mask;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_and_return(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    target->value &= mask;
    return target->value;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_and_return_old(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    okl4_atomic_plain_word_t old = target->value;
    target->value = old & mask;
    return old;
}

static inline void
okl4_atomic_or(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    target->value |= mask;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_or_return(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    target->value |= mask;
    return target->value;
}

static inline void
okl4_atomic_xor(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    target->value ^= mask;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_xor_return(okl4_atomic_word_t* target, okl4_atomic_plain_word_t mask)
{
    target->value ^= mask;
    return target->value;
}

/* Bitfield operations. */

static inline void
okl4_atomic_set_bit(okl4_atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_word_t) * 8);
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_word_t) * 8));

    target[word].value |= mask;
}

static inline void
okl4_atomic_clear_bit(okl4_atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_word_t) * 8);
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_word_t) * 8));

    target[word].value &= ~mask;
}

static inline int
okl4_atomic_clear_bit_return_old(okl4_atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_word_t) * 8);
    okl4_atomic_plain_word_t old;
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_word_t) * 8));

    old = target[word].value;
    target[word].value = old & (~mask);

    return (old & mask) != 0;
}

static inline void
okl4_atomic_change_bit(okl4_atomic_word_t *target, unsigned long bit)
{
    int word = bit / (sizeof(okl4_atomic_word_t) * 8);
    okl4_atomic_plain_word_t mask = 1 << (bit % (sizeof(okl4_atomic_word_t) * 8));

    target[word].value ^= mask;
}


/* Arithmetic operations. */
static inline void
okl4_atomic_add(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v)
{
    target->value += v;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_add_return(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v)
{
    target->value += v;
    return target->value;
}

static inline void
okl4_atomic_sub(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v)
{
    target->value -= v;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_sub_return(okl4_atomic_word_t *target, okl4_atomic_plain_word_t v)
{
    target->value -= v;
    return target->value;
}

static inline void
okl4_atomic_inc(okl4_atomic_word_t *target)
{
    target->value++;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_inc_return(okl4_atomic_word_t *target)
{
    target->value++;
    return target->value;
}


static inline void
okl4_atomic_dec(okl4_atomic_word_t *target)
{
    target->value--;
}

static inline okl4_atomic_plain_word_t
okl4_atomic_dec_return(okl4_atomic_word_t *target)
{
    target->value--;
    return target->value;
}

/* General operations. */

static inline int
okl4_atomic_compare_and_set(okl4_atomic_word_t *target,
                       okl4_atomic_plain_word_t old_val,
                       okl4_atomic_plain_word_t new_val)
{
    if (target->value == old_val) {
        target->value = new_val;
        return 1;
    }

    return 0;
}

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

#endif /* __ARCH_ATOMIC_OPS_UNSAFE_GENERIC_H__ */
