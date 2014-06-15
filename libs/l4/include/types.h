#ifndef __L4__TYPES_H__
#define __L4__TYPES_H__

#include <l4/arch/types.h>

typedef L4_Word_t word_t;


typedef word_t L4_Bool_t;


#define L4_INLINE               static inline
#ifndef INLINE
#define INLINE L4_INLINE
#endif

/** @todo FIXME Source from libc's limit.h once we can - awiggins. */
#define CHAR_BIT 8
#define WORD_T_BIT (sizeof (word_t) * CHAR_BIT)
#define L4_BITS_PER_WORD WORD_T_BIT

#include <l4/macros.h> /* BITFIELD() and SHUFFLE() macros. */

#define L4_BITFIELD2(t,a,b)               BITFIELD2(t,a,b)
#define L4_BITFIELD3(t,a,b,c)             BITFIELD3(t,a,b,c)
#define L4_BITFIELD4(t,a,b,c,d)           BITFIELD4(t,a,b,c,d)

#endif /* !__L4__TYPES_H__ */
