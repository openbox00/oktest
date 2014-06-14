/*
 * Description:  Memory mapping API class definitions
 */
#ifndef __MAP_H__
#define __MAP_H__
#include <l4/map_types.h>
#include <arch/pgent.h>

class phys_desc_t
{
public:
    inline word_t get_raw()
        { return raw; }

    inline u64_t get_base()
        { return ((u64_t)x.base) << __L4_MAP_SHIFT; }

    inline void set_base(u64_t base)
        { x.base = base >> __L4_MAP_SHIFT; }

    inline word_t get_attributes()
        { return x.attr; }

private:
    union {
        struct {
            BITFIELD2(word_t,
                    attr        : 6,
                    base        : BITS_WORD - 6);
        } x;
        word_t raw;
    };
};


#endif /* __MAP_H__ */
