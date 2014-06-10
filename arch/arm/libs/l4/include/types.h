/*
 * Description:   ARM specific type declararions
 */
#ifndef __L4__ARM__TYPES_H__
#define __L4__ARM__TYPES_H__

#define L4_32BIT

#if defined(ENDIAN_BIG)
#define L4_BIG_ENDIAN
#elif defined(ENDIAN_LITTLE)
#define L4_LITTLE_ENDIAN
#else
#error No endianness configured
#endif

typedef unsigned long long L4_Word64_t;
typedef unsigned long L4_Word32_t;
typedef unsigned short L4_Word16_t;
typedef unsigned char L4_Word8_t;

typedef unsigned long L4_Word_t;
typedef unsigned long L4_PtrSize_t;
typedef unsigned int L4_Size_t;

typedef short L4_Int16_t;

#define PRI_X_WORD "lx"
#define PRI_D_WORD "ld"

#endif /* !__L4__ARM__TYPES_H__ */
