/*
 * Description: Synchronization primitives.
 */

#ifndef __SYNC_H__
#define __SYNC_H__

#include <l4.h>

#if defined(CONFIG_MDOMAINS) ||  defined(CONFIG_MUNITS)

/*
 * SMT architectures should define their own implementation of
 * `spinlock_t'.
 */
#include <arch/sync.h>

DECLARE_SPINLOCK(privileged_syscall_lock);
#define LOCK_PRIVILEGED_SYSCALL()   privileged_syscall_lock.lock()
#define UNLOCK_PRIVILEGED_SYSCALL() privileged_syscall_lock.unlock()

#else /* !CONFIG_MDOMAINS && !CONFIG_MUNITS */

#define DEFINE_SPINLOCK(name) spinlock_t name
#define DECLARE_SPINLOCK(name) extern spinlock_t name

struct spinlock_t {

    /** Acquire this lock. */
    INLINE void lock();

    /** Release this lock. */
    INLINE void unlock();

    /** Initialise this lock. */
    INLINE void init();

    /** Return if this spinlock is currently locked or not. */
    INLINE bool is_locked();

};

INLINE void
spinlock_t::lock()
{
}

INLINE void
spinlock_t::unlock()
{
}

INLINE void
spinlock_t::init()
{
}

INLINE bool
spinlock_t::is_locked()
{
    return false;
}

#define LOCK_PRIVILEGED_SYSCALL()
#define UNLOCK_PRIVILEGED_SYSCALL()


#endif

#endif /* !__SYNC_H__ */
