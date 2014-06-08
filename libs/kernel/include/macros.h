/*
 * Description:   Omnipresent macro definitions
 */

#ifndef __MACROS_H__
#define __MACROS_H__

#include <compat/c.h>
#include <l4/macros.h>

/* use this to place code/data in a certain section */
#if defined(__APPLE_CC__)
#define UNIT(x)                 SECTION("__DATA, " x)
#define CALIGN_16 ".align 4"
#else
#define UNIT(x)                 SECTION(".data." x)
#define CALIGN_16 ".align 16"
#endif

/*
  There is a gcc extension that allows you to provid asm names for
  declared methods. E.g:
  class foo {
     void bar(void) __asm__("bar");
  }
  Instead of directly using this we provide the ASM_NAME macro so it becomes:
  class foo {
     void bar(void) ASM_NAME("bar");
  }
  This is much more readable, and allows us to easily ignore it when using flint.
*/
#ifdef _lint
#define ASM_NAME(x)
#else
#define ASM_NAME(x) __asm__(x)
#endif

#ifdef _lint
#define ASM_REG(x)
#else
#define ASM_REG(x) asm(x)
#endif

/*
 * Produce a link-time error of the given string. If the LINK_ERROR()
 * macro is not compiled away by the dead-code optimiser, a link-time
 * error will be produced.
 *
 * The string must be a valid function name (and can not contain spaces,
 * for instance.)
 */
#if defined(__GNUC__)
#define LINK_ERROR(x)                              \
        do {                                       \
            extern NORETURN void __LINK_ERROR__##x(void);   \
            __LINK_ERROR__##x();                   \
        } while (0)
#else
#define LINK_ERROR(x)
#endif

/*
 * Ensure that the given token is a compile-time constant, or
 * generate a link-time error.
 */
#if defined(__GNUC__)
#define ENSURE_CONSTANT(x, msg)                    \
        do {                                       \
            if (!__constant_p(x)) {                \
                LINK_ERROR(msg);                   \
            }                                      \
        } while (0)
#else
#define ENSURE_CONSTANT(x, msg)
#endif

/*
 * Mark code as unreachable, and remove it for fussy compilers.
 */
#if defined(__RVCT_GNU__)
#define UNREACHABLE(x)
#else
#define UNREACHABLE(x)          x
#endif

/*
 * Mark code as unreachable, and produce a link-time error if it is
 * actually reached.
 */
#if defined(__GNUC__)
#if !defined(ASSEMBLY)
extern void __unreachable_code_reached(void);
#endif
/*
 * The compiler will optimise away NOTREACHED() calls if they are in
 * fact not reachable. If they can be reached, the compiler will
 * generate a link-time error with the non-existant symbol
 * "__unreachable_code_reached".
 */
#define NOTREACHED()                                      \
        do {                                              \
            __unreachable_code_reached();                 \
            for (;;);                                     \
        } while(0)
#else
#define NOTREACHED() \
        panic("Unreachable code reached.\n")
#endif

#undef INLINE

#if defined(__cplusplus)
#define C_LINKAGE extern "C"
#define INLINE inline
#else
#define C_LINKAGE
#define INLINE static inline
/* Kernel doesn't have access to stdbool.h so re-define bool type here for
 * C code.
 */
#if !defined(ASSEMBLY)
typedef word_t bool;
#define false 0
#define true (!false)
#endif
#endif

/* Functions for critical path optimizations */
#define EXPECT_FALSE(x)         __unlikely(x)
#define EXPECT_TRUE(x)          __likely(x)
#define EXPECT_VALUE(x,val)     __expect((x), (val))

/* Convenience functions for memory sizes. */
#define KB(x)   ((x) * 1024)
#define MB(x)   ((x) * 1024*1024)
#define GB(x)   ((x) * 1024*1024*1024)

/* Offset for struct fields. */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define offsetof(type, field)   __builtin_offsetof(type, field)
#else
#define offsetof(type, field)   ((word_t) (&((type *) 0)->field))
#endif

/* Turn preprocessor symbol definition into string */
#define MKSTR(sym)      MKSTR2(sym)
#define MKSTR2(sym)     #sym

/* Safely "append" an UL suffix for also asm values */
#if defined(ASSEMBLY)
#define __UL(x)         x
#else
#define __UL(x)         x##UL
#endif

#define TWOSCOMP(X) ((~(X))+1)

#endif /* !__MACROS_H__ */
