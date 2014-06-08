#ifndef UTCB_ALLOC_H
#define UTCB_ALLOC_H

#include <okl4/utcb.h>

void _okl4_utcb_alloc(okl4_utcb_area_t * utcb_area,
        okl4_utcb_item_t * utcb_item, okl4_word_t unit);

#endif /* !UTCB_ALLOC_H */

