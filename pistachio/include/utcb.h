#ifndef __UTCB_H__
#define __UTCB_H__

#include <l4/arch/vregs.h> /* UTCB Layout. */

typedef okl4_atomic_word_t notify_bits_t;

class utcb_t {
public:
    UTCB_STRUCT;
};



#endif /* !__UTCB_H__ */
