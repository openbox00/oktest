/*
 * Description:   Omnipresent macro definitions.
 */

#ifndef __L4__MACROS_H__
#define __L4__MACROS_H__

// FIXME: Unify CONFIG_BIGENDIAN/L4_BIG_ENDIAN - AGW.
#if (defined(CONFIG_BIGENDIAN) || defined(L4_BIG_ENDIAN))
#define BITFIELD2(t,a,b)                t b; t a
#define BITFIELD3(t,a,b,c)              t c; t b; t a
#define BITFIELD4(t,a,b,c,d)            t d; t c; t b; t a
#define BITFIELD5(t,a,b,c,d,e)          t e; t d; t c; t b; t a
#define BITFIELD6(t,a,b,c,d,e,f)        t f; t e; t d; t c; t b; t a
#define BITFIELD7(t,a,b,c,d,e,f,g)      t g; t f; t e; t d; t c; t b; t a
#define BITFIELD8(t,a,b,c,d,e,f,g,h)    t h; t g; t f; t e; t d; t c; t b; t a
#define BITFIELD9(t,a,b,c,d,e,f,g,h,i)  t i; t h; t g; t f; t e; t d; t c; t b; t a
#define BITFIELD10(t,a,b,c,d,e,f,g,h,i,j) t j; t i; t h; t g; t f; t e; t d; t c; t b; t a
#define BITFIELD11(t,a,b,c,d,e,f,g,h,i,j,k) t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a
#define BITFIELD12(t,a,b,c,d,e,f,g,h,i,j,k,l) t l; t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a
#define BITFIELD13(t,a,b,c,d,e,f,g,h,i,j,k,l,m) t m; t l; t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a
#define BITFIELD14(t,a,b,c,d,e,f,g,h,i,j,k,l,m,n) t n; t m; t l; t k; t j; t i; t h; t g; t f; t e; t d; t c; t b; t a

#define BITFIELD17(t,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) t q; t p; t o; t n; t m; t l; t k; t j;t i; t h; t g; t f; t e; t d; t c; t b; t a

#define SHUFFLE2(a,b)                   b,a
#define SHUFFLE3(a,b,c)                 c,b,a
#define SHUFFLE4(a,b,c,d)               d,c,b,a
#define SHUFFLE5(a,b,c,d,e)             e,d,c,b,a
#define SHUFFLE6(a,b,c,d,e,f)           f,e,d,c,b,a
#define SHUFFLE7(a,b,c,d,e,f,g)         g,f,e,d,c,b,a
#elif (defined(CONFIG_LITTLEENDIAN) || defined(L4_LITTLE_ENDIAN))
#define BITFIELD2(t,a,b)                t a; t b
#define BITFIELD3(t,a,b,c)              t a; t b; t c
#define BITFIELD4(t,a,b,c,d)            t a; t b; t c; t d
#define BITFIELD5(t,a,b,c,d,e)          t a; t b; t c; t d; t e
#define BITFIELD6(t,a,b,c,d,e,f)        t a; t b; t c; t d; t e; t f
#define BITFIELD7(t,a,b,c,d,e,f,g)      t a; t b; t c; t d; t e; t f; t g
#define BITFIELD8(t,a,b,c,d,e,f,g,h)    t a; t b; t c; t d; t e; t f; t g; t h
#define BITFIELD9(t,a,b,c,d,e,f,g,h,i)  t a; t b; t c; t d; t e; t f; t g; t h; t i
#define BITFIELD10(t,a,b,c,d,e,f,g,h,i,j) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j
#define BITFIELD11(t,a,b,c,d,e,f,g,h,i,j,k) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k
#define BITFIELD12(t,a,b,c,d,e,f,g,h,i,j,k,l) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l
#define BITFIELD13(t,a,b,c,d,e,f,g,h,i,j,k,l,m) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l; t m
#define BITFIELD14(t,a,b,c,d,e,f,g,h,i,j,k,l,m,n) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l; t m; t n
#define BITFIELD17(t,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) t a; t b; t c; t d; t e; t f; t g; t h; t i; t j; t k; t l; t m; t n; t o; t p; t q
#define SHUFFLE2(a,b)                   a,b
#define SHUFFLE3(a,b,c)                 a,b,c
#define SHUFFLE4(a,b,c,d)               a,b,c,d
#define SHUFFLE5(a,b,c,d,e)             a,b,c,d,e
#define SHUFFLE6(a,b,c,d,e,f)           a,b,c,d,e,f
#define SHUFFLE7(a,b,c,d,e,f,g)         a,b,c,d,e,f,g
#else
#error "Endianess not defined!"
#endif


#endif /* !__L4__MACROS_H__ */
