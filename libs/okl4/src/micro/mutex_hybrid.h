#include <okl4/types.h>
#include <okl4/mutex.h>

#include <l4/types.h>
#include <l4/mutex.h>
#include <l4/thread.h>

/**
 *  @file Micro hybrid mutex utility functions.
 *
 *  @internal
 */

/* The bit used to indicate contention on the lock. */
#define MUTEX_CONTENTION_BIT (WORD_T_BIT - 1)

/* A free mutex. */
#define MUTEX_FREE (~0UL)

/* Bitmask to set/check if a mutex is contended. */
#define MUTEX_CONTENDED ((word_t)1 << MUTEX_CONTENTION_BIT)

/* Get the current thread's handle for the hybrid mutex protocol. */
INLINE okl4_word_t
mutex_my_handle(void)
{
    return L4_MyMutexThreadHandle().raw;
}

/* Return the handle of the thread that currently holds the given mutex, or
 * (-1) to indicate free. */
INLINE word_t
mutex_get_holder(okl4_mutex_t *mutex)
{
    /* To convert the stored value into a thread handle, we need to set the top
     * bit. */
    return okl4_atomic_read(&(mutex->value)) | MUTEX_CONTENDED;
}

/* Determine if we currently hold the given mutex. */
INLINE int
mutex_is_held_by_self(okl4_mutex_t *mutex)
{
    return mutex_my_handle() == mutex_get_holder(mutex);
}

