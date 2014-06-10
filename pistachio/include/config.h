/*
 * Description:   global configuration, include wrapper
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <kernel/config.h>

#define CONFIG_MAX_UNITS_PER_DOMAIN CONFIG_NUM_UNITS / CONFIG_NUM_DOMAINS

/**
 * SMP derives
 */
#if defined(CONFIG_NUM_DOMAINS) && CONFIG_NUM_DOMAINS > 1
#define CONFIG_MDOMAINS
#endif

#if defined(CONFIG_NUM_UNITS) && CONFIG_NUM_UNITS > 1
#define CONFIG_MUNITS
#endif

#if defined(CONFIG_IS_32BIT)
# define L4_GLOBAL_THREADNO_BITS                18
# define L4_GLOBAL_INTRNO_BITS                  18
# define L4_GLOBAL_VERSION_BITS                 14
# define L4_FPAGE_BASE_BITS                     22
#elif defined(CONFIG_IS_64BIT)
# define L4_GLOBAL_THREADNO_BITS                32
# define L4_GLOBAL_INTRNO_BITS                  32
# define L4_GLOBAL_VERSION_BITS                 32
# define L4_FPAGE_BASE_BITS                     54
#else
# error undefined architecture width (32/64bit?)
#endif

#define DEFAULT_TIMESLICE_LENGTH        10000
#define MAX_PRIO                        255
#define DEFAULT_PRIORITY                0
#define DEFAULT_CONTEXT_BITMASK         ~(0UL)

#if defined(CONFIG_DEBUG)
#if defined(CONFIG_KDEBUG_TIMER) || defined(CONFIG_TRACEBUFFER)
#define CONFIG_KEEP_CURRENT_TIME
#endif
#endif

/*
 * root server configuration
 */
#define ROOT_MAX_THREADS                256

#if defined(CONFIG_IS_32BIT)
#define L4_WORD_SIZE    4
#elif defined(CONFIG_IS_64BIT)
#define L4_WORD_SIZE    8
#endif

/*
 * Size of the bounce buffer for the remote memory copy feature.
 *
 * It should be less than or equal the minimum page size. Otherwise, it is a
 * waste of space since the unit of copying is min(REMOTE_MEMCPY_BUFSIZE,
 * minimum page size).
 *
 * TODO : Ensure it during compilation time
 */
#define REMOTE_MEMCPY_BUFSIZE           0x400UL

#include <arch/config.h>

#ifndef ARCH_MAX_SPACES
// no space-id limit
#define ARCH_MAX_SPACES     0
#endif

#endif /* !__CONFIG_H__ */
