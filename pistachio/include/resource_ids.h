/*
 * Description: Resource id ownership
 */

#ifndef __RESOURCE_IDS_H__
#define __RESOURCE_IDS_H__

class resource_ids_t {

private:
    u16_t   base;
    u16_t   size;

public:
    bool is_valid(u16_t index) {
        return (index >= base) && (index < (base + size));
    }

    void set_range(word_t newbase, word_t newsize) {
        ASSERT(DEBUG, (newbase + newsize) < (1UL << 16));
        base = newbase;
        size = newsize;
    }

    word_t get_base(void) {
        return base;
    }
    word_t get_size(void) {
        return size;
    }
};

#endif    /*__RESOURCE_IDS_H__*/
