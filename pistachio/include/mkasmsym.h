/*
 * Description:   Macro for generating object file output that can be
 */
#ifndef __MKASMSYM_H__
#define __MKASMSYM_H__

#if defined(__GNUC__)
#define MKASMSYM_START          void mkasmsym() {
#define MKASMSYM_END            }
#define MKASMSYM(sym, val)      __asm__ __volatile__ ("SYM " #sym " %n0" : : "i" (-(val)))

#elif defined(__RVCT_GNU__)
#define MKASMSYM_START          __asm void mkasmsym() {
#define MKASMSYM_END            }
#define MKASMSYM(sym, val)      SYM sym __cpp(val)
#endif

#endif /* !__MKASMSYM_H__ */
