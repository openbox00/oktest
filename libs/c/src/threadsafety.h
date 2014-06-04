/*
 * Thread safety compatability layer.
 *
 * This interface attempts to abstract away differences in locking mechanisms
 * for differnt interfaces.
 */

#ifndef _THREADSAFETY_H_
#define _THREADSAFETY_H_

#if defined(NANOKERNEL)
/* Currently, Nano only uses libokl4 locks. */
#define LIBOKL4_LOCKS
#else
/* Currently, Micro only uses libmutex locks. */
#define LIBMUTEX_LOCKS
#endif

#if defined(THREAD_SAFE) && defined(LIBOKL4_LOCKS)

/*
 * Using 'libokl4' to perform locking operations.
 */

#include <okl4/mutex.h>
#include <assert.h>

/*
 * Setup
 */

/* The data type for a mutex. */
#define OKL4_MUTEX_TYPE  okl4_mutex_t

/* Initialise a mutex. */
#define OKL4_MUTEX_INIT(x) \
    do { \
        int _okl4_err; \
        okl4_mutex_attr_t _okl4_attr; \
        okl4_mutex_attr_init(&_okl4_attr); \
        okl4_mutex_attr_setiscounted(&_okl4_attr, 1); \
        _okl4_err = okl4_mutex_create(x, &_okl4_attr); \
        assert(!_okl4_err); \
    } while (0)

/* Destroy a mutex. */
#define OKL4_MUTEX_FREE(x) \
    okl4_mutex_delete(x);

/*
 * Stream locking / unlocking.
 */
#define lock_stream(s) \
    do { \
        if (!(s)->mutex_initialised) { \
            OKL4_MUTEX_INIT(&(s)->mutex); \
            (s)->mutex_initialised = 1; \
        } \
        okl4_mutex_lock(&(s)->mutex); \
    } while (0)

#define unlock_stream(s) \
    do {\
        assert((s)->mutex_initialised); \
        okl4_mutex_unlock(&(s)->mutex); \
    } while (0)

/*
 * Mutex subsystem locking / unlocking.
 */

#define MALLOC_LOCK \
    okl4_mutex_lock(&malloc_mutex)

#define MALLOC_UNLOCK \
    okl4_mutex_unlock(&malloc_mutex)

extern OKL4_MUTEX_TYPE malloc_mutex;

#elif defined(THREAD_SAFE) && defined(LIBMUTEX_LOCKS)

/*
 * Using 'libmutex' to perform locking operations.
 */

//#include <mutex/mutex.h>

/*
 * Setup
 */

/* The data type for a mutex. */
#define OKL4_MUTEX_TYPE  struct okl4_libmutex

/* Initialise a mutex. */
#define OKL4_MUTEX_INIT(x) \
    okl4_libmutex_init(x)

/* Destroy a mutex. */
#define OKL4_MUTEX_FREE(x) \
    okl4_libmutex_free(x)

/*
 * Stream locking / unlocking.
 */
#define lock_stream(s) \
    do { \
        if (!(s)->mutex_initialised) {\
            okl4_libmutex_init(&(s)->mutex); \
            (s)->mutex_initialised = 1; \
        } \
        okl4_libmutex_count_lock(&(s)->mutex); \
    } while (0)

#define unlock_stream(s) \
    do {\
        assert((s)->mutex_initialised); \
        okl4_libmutex_count_unlock(&(s)->mutex);\
    } while (0)

/*
 * Mutex subsystem locking / unlocking.
 */

#define MALLOC_LOCK \
    okl4_libmutex_count_lock(&malloc_mutex)

#define MALLOC_UNLOCK \
    okl4_libmutex_count_unlock(&malloc_mutex)

extern OKL4_MUTEX_TYPE malloc_mutex;

#elif !defined(THREAD_SAFE)

/* Setup / teardown. */
#define OKL4_MUTEX_TYPE  int
#define OKL4_MUTEX_INIT(x) do { } while (0)
#define OKL4_MUTEX_FREE(x) do { } while (0)

/* Stream locking / unlocking. */
#define lock_stream(s)    do { } while (0)
#define unlock_stream(s)  do { } while (0)

/* Mutex subsystem locking / unlocking. */
#define MALLOC_LOCK       do { } while (0)
#define MALLOC_UNLOCK     do { } while (0)

#else
#error "Unknown locking mechanism in use."
#endif

#endif /* !_THREADSAFETY_H_ */

