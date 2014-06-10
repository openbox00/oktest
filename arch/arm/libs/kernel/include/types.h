/*
 * Description:   ARM-specific basic data types
 */
#ifndef __ARCH__ARM__TYPES_H__
#define __ARCH__ARM__TYPES_H__

#define L4_32BIT
#undef  L4_64BIT

#if !defined(ASSEMBLY)
typedef unsigned long long      u64_t;
typedef unsigned long           u32_t;
typedef unsigned short          u16_t;
typedef unsigned char           u8_t;

typedef signed long long        s64_t;
typedef signed long             s32_t;
typedef signed short            s16_t;
typedef signed char             s8_t;

#define GLOBAL_DEC(type, x)
#define GLOBAL_DEF(x)
#define GLOBAL(x)       (get_arm_globals()->x)
#endif

#endif /* !__ARCH__ARM__TYPES_H__ */
