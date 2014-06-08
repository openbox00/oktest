#ifndef __OKL4_MESSAGE_HELPERS_H__
#define __OKL4_MESSAGE_HELPERS_H__

#include <okl4/types.h>
#include <l4/ipc.h>

/**
 *  Copy a buffer into L4 virtual message registers.
 */
void _okl4_message_copy_buff_to_mrs(void *buff, okl4_word_t num_bytes);

/**
 *  Copy L4 virtual message registers into the given buffer.
 */
void _okl4_message_copy_mrs_to_buff(void *buff, okl4_word_t num_bytes);

/**
 *  Covert an L4 IPC error code into a libokl4 error code.
 */
int _okl4_message_errno(L4_MsgTag_t tag, okl4_word_t code);

/* Grab minimum of two values. */
inline static okl4_word_t
min(okl4_word_t a, okl4_word_t b)
{
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

/* Round up 'n' to the next value of 'r'. */
#define ROUND_UP(n, r) ((((n) + (r) - 1) / (r)) * (r))

#endif /* !__OKL4_MESSAGE_HELPERS_H__ */

