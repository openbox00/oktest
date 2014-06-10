/*
 * Description: Cache control API class definitions
 */
#ifndef __API__V4__CACHE_H__
#define __API__V4__CACHE_H__

#define CACHE_CTL_MASKL1               1
#define CACHE_CTL_MASKL2               2
#define CACHE_CTL_MASKL3               4
#define CACHE_CTL_MASKL4               8
#define CACHE_CTL_MASKL5              16
#define CACHE_CTL_MASKL6              32
#define CACHE_CTL_MASKALLLS           63

enum cacheop_e {
    cop_arch        = 0,
    cop_flush_range = 1,
    cop_flush_I_all = 4,
    cop_flush_D_all = 5,
    cop_flush_all   = 6,
    cop_lock        = 8,
    cop_unlock      = 9,
};

#include <kernel/cache.h>

class cache_control_t
{
public:
    inline word_t highest_item()
        { return x.n; }

    inline cacheop_e operation()
        { return (cacheop_e)x.op; }

    inline word_t cache_level_mask()
        { return x.lx; }

    inline void operator = (word_t raw)
        { this->m_raw = raw; }
private:
    union {
        word_t m_raw;
        struct {
            BITFIELD4(word_t,
                    n           : 6,
                    op          : 6,
                    lx          : 6,
                    __res       : BITS_WORD - 18);
        } x;
    };
};

#include <arch/cache.h>

#endif /* __API__V4__CACHE_H__ */
