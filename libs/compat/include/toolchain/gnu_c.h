#ifndef __GNU_C_H__
#define __GNU_C_H__

/*
 * Attributes
 */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define ALIGNED(x)              __attribute__((__aligned__(x)))
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5)
#define CONST                   __attribute__((__const__))
#else
#define CONST
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#define DEPRECATED              __attribute__((__deprecated__))
#else
#define DEPRECATED
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 0)
#define MALLOC                  __attribute__((__malloc__))
#else
#define MALLOC
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5)
#define NORETURN                __attribute__((__noreturn__))
#else
#define NORETURN
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define PACKED(x)               x __attribute__((__packed__))
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 0)
#define PURE                    __attribute__((__pure__))
#else
#define PURE
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 6)
#define SECTION(x)              __attribute__((__section__(x)))
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define TRANSPARENT_UNION       __attribute__((__transparent_union__))
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 8)
#define UNUSED                  __attribute__((__unused__))
#else
#define UNUSED
#endif

#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define WEAK                    __attribute__((__weak__))
#else
#define WEAK
#endif

/*
 * Builtins
 */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 0 && __GNUC_PATCHLEVEL__ >= 1)
#define __constant_p(x)         __builtin_constant_p(x)
#else
#define __constant_p(x)         (0)
#endif

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 0)
#define __expect(x, c)          __builtin_expect(x, c)
#else
#define __expect(x, c)          (x)
#endif

#define __frame_address()       __builtin_frame_address(0)
#define __likely(x)             __expect(!!(x), 1)
#ifdef BROKEN_BUILTIN_RETURN_ADDR
#include <compat/arch/builtin_return_addr.h>
#else
#define __return_address()      __builtin_return_address(0)
#endif

#define __unlikely(x)           __expect(!!(x), 0)

#define __clz(x)                __builtin_clz(x)

/*
 * Macros
 */
#ifndef __FUNCTION__
#define __FUNCTION__            __FUNCTION__
#endif
#define __PRETTY_FUNCTION__     __PRETTY_FUNCTION__
#define __func__                __func__

/*
 * Modifiers
 */
#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1) && (!defined(MACHINE_IA32_PC99))
#define RESTRICT                __restrict
#else
#define RESTRICT
#endif

#endif
