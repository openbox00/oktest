#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <kernel/config.h>

# define L4_GLOBAL_THREADNO_BITS                18
# define L4_GLOBAL_INTRNO_BITS                  18
# define L4_GLOBAL_VERSION_BITS                 14
# define L4_FPAGE_BASE_BITS                     22

#define DEFAULT_TIMESLICE_LENGTH        10000
#define MAX_PRIO                        255
#define DEFAULT_PRIORITY                0
#define DEFAULT_CONTEXT_BITMASK         ~(0UL)

#define ROOT_MAX_THREADS                256
#define L4_WORD_SIZE    4

#define REMOTE_MEMCPY_BUFSIZE           0x400UL

#include <arch/config.h>

#endif /* !__CONFIG_H__ */
