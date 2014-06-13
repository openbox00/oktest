#ifndef __ATOMIC_OPS_H__
#define __ATOMIC_OPS_H__

typedef L4_Word_t okl4_atomic_plain_word_t;

typedef struct {
    volatile okl4_atomic_plain_word_t value;
} okl4_atomic_word_t;

#endif /* __ATOMIC_OPS_H__ */
