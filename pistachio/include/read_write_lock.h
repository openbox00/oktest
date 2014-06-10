/*
 * Read/Write Lock
 */
#ifndef __READ_WRITE_LOCK_H__
#define __READ_WRITE_LOCK_H__

#if defined(CONFIG_MDOMAINS) ||  defined(CONFIG_MUNITS)

/* Include the architecture-specific read/write lock. */
#include <arch/read_write_lock.h>

#else /* !CONFIG_MDOMAINS && !CONFIG_MUNITS */

/**
 * Read/Write lock.
 *
 * Only a single writer may hold the lock at any time, but an
 * unlimited number of reader. If a writing thread is waiting
 * on the lock, they have preference over the reading threads.
 */
struct read_write_lock_t
{
    /** Initialise the read/write lock. */
    void init();

    /**
     * Acquire this lock for reading. Other readers may still acquire
     * the lock.
     */
    void lock_read(void);

    /**
     * Use this function only if the reader already holds this lock  
     * and needs to acquire the same lock for reading a second time.
     *
     * Acquire this lock for reading even though there are pending writers. 
     * Other readers may still acquire the lock.
     */
    void lock_read_already_held(void);

    /**
     * Try acquire this lock for reading. Other readers may still acquire
     * the lock.
     *
     * @returns true if lock aquired, false if lock held
     */
    bool try_lock_read(void);

    /**
     * Release this lock for reading.
     */
    void unlock_read(void);

    /**
     * Acquire this lock for writing. The lock will not be acquired
     * until all other readers or writers have released the lock. No new
     * readers will be allowed to take the lock until we fully acquire
     * it and release it.
     */
    void lock_write(void);

    /**
     * Try aquire writer lock
     *
     * @returns true if lock aquired, false if lock held
     */
    bool try_lock_write(void);

    /**
     * Release the lock for writing.
     */
    void unlock_write(void);

    /** Determine if the lock is currently held by any thread. */
    bool is_locked(void);

#if defined(CONFIG_DEBUG)
    word_t locked;
#endif
};

INLINE void
read_write_lock_t::init()
{
#if defined(CONFIG_DEBUG)
    locked = 0;
#endif
}

INLINE void
read_write_lock_t::lock_read(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked >= 0);
    locked++;
#endif
}

INLINE void
read_write_lock_t::lock_read_already_held(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked >= 0);
    locked++;
#endif
}

INLINE bool
read_write_lock_t::try_lock_read(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked >= 0);
    locked++;
#endif
    return true;
}

INLINE void
read_write_lock_t::unlock_read(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked > 0);
    locked--;
#endif
}

INLINE void
read_write_lock_t::lock_write(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked == 0);
    locked = (word_t)-1;
#endif
}

INLINE bool
read_write_lock_t::try_lock_write(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked == 0);
    locked = (word_t)-1;
#endif
    return true;
}

INLINE void
read_write_lock_t::unlock_write(void)
{
#if defined(CONFIG_DEBUG)
    ASSERT(ALWAYS, (int)locked == -1);
    locked = 0;
#endif
}

INLINE bool
read_write_lock_t::is_locked(void)
{
#if defined(CONFIG_DEBUG)
    return locked;
#else
    return false;
#endif
}

/** Import an externally-declared read_write_lock. */
#define DECLARE_READ_WRITE_LOCK(name) extern read_write_lock_t name

/** Definie a statically-allocated read_write_lock. */
#if defined(_lint)
#define DEFINE_READ_WRITE_LOCK(name) \
        read_write_lock_t name
#elif defined(CONFIG_DEBUG)
#define DEFINE_READ_WRITE_LOCK(name) \
        read_write_lock_t name = \
                ((read_write_lock_t){0})
#else
#define DEFINE_READ_WRITE_LOCK(name) \
        read_write_lock_t name
#endif

#endif /* !CONFIG_MDOMAINS && !CONFIG_MUNITS */

#endif /* !__READ_WRITE_LOCK_H__ */

