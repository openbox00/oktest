/*
 * Description:  Physical Segments and Segments Table
 */
#ifndef __PHYS_SEGMENT_H__
#define __PHYS_SEGMENT_H__

#include <arch/pgent.h>

extern const word_t hw_pgshifts[];

#define MIN_PAGE_SHIFT  hw_pgshifts[pgent_t::size_min]

class phys_segment_t
{
public:
    inline bool attribs_allowed(memattrib_e attr)
    {
        word_t physattr;
        return (this->base.attribs & physattr) == physattr;
    }

    inline bool is_contained(word_t addr_offset, word_t map_size)
    {
        word_t offset = addr_offset >> MIN_PAGE_SHIFT;
        word_t size = map_size >> MIN_PAGE_SHIFT;

        if ((offset + size) <= this->size.size) {
            return true;
        } else {
            return false;
        }
    }

    inline void set_base(u64_t offset)
    {
        base.base = offset >> MIN_PAGE_SHIFT;
    }

    inline void set_size(u64_t s)
    {
        size.size = s >> MIN_PAGE_SHIFT;
    }

    inline u64_t get_base()
    {
        return (u64_t)base.base << MIN_PAGE_SHIFT;
    }

    inline u64_t get_size()
    {
        return (u64_t)size.size << MIN_PAGE_SHIFT;
    }

    inline void set_rwx(word_t rwx)
    {
        size.rwx = rwx;
    }

    inline word_t get_rwx()
    {
        return size.rwx;
    }

    inline void set_attribs(word_t attr)
    {
        base.attribs = attr;
    }

    inline word_t get_attribs()
    {
        return base.attribs;
    }

private:
    union {
        struct {
            BITFIELD2(word_t,
                    attribs     : 8,
                    base        : BITS_WORD - 8);
        } base;
        word_t raw_base;
    };
    union {
        struct {
            BITFIELD3(word_t,
                    rwx         : 4,
                    _res        : 6,
                    size        : BITS_WORD - 10);
        } size;
        word_t raw_size;
    };
};

class segment_list_t
{
public:
    inline bool valid_segment(word_t id)
    {
        return id < num_segments;
    }

    inline phys_segment_t *lookup_segment(word_t id)
    {
        if (!valid_segment(id)) {
            return NULL;
        }
        return &segments[id];
    }

    inline word_t get_num_segments(void)
    {
        return num_segments;
    }

    /* XXX: This will be removed soon */
    inline void set_num_segments(word_t n)
    {
        num_segments = n;
    }
private:
    word_t num_segments;
public:
    phys_segment_t segments[1];
};

#undef MIN_PAGE_SHIFT

#endif /* __PHYS_SEGMENT_H__ */
