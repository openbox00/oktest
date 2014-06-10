/*
 * Description:  Memory mapping API class definitions
 */
#ifndef __MAP_H__
#define __MAP_H__

#include <l4/map_types.h>

#include <arch/pgent.h>

class size_desc_t
{
public:
    inline word_t get_raw()
        { return raw; }

    inline void set_raw(word_t val)
        { this->raw = val; }

    inline word_t get_page_size()
        { return this->x.page_size; }

    inline word_t get_num_pages()
        { return this->x.num_pages; }

    inline word_t get_valid()
        { return this->x.valid; }

    inline word_t get_rwx()
        { return this->x.rwx; }
private:
    union {
        size_descriptor_t x;
        word_t raw;
    };
};

class segment_desc_t
{
public:
    inline word_t get_raw()
        { return raw; }

    inline void set_raw(word_t val)
        { this->raw = val; }

    inline word_t get_offset()
        { return this->x.offset << __L4_MAP_SHIFT; }

    inline void set_offset(word_t offset)
        { this->x.offset = offset >> __L4_MAP_SHIFT; }

    inline word_t get_segment()
        { return this->x.segment; }
private:
    union {
        segment_descriptor_t x;
        word_t raw;
    };
};

class virt_desc_t
{
public:
    inline word_t get_raw()
        { return raw; }

    inline void set_raw(word_t val)
        { this->raw = val; }

    inline word_t get_base()
        { return x.vbase << __L4_MAP_SHIFT; }

    inline void set_attributes(word_t attr)
        { this->x.attr = attr; }

    inline memattrib_e get_attributes()
        { return (memattrib_e)this->x.attr; }
private:
    union {
        virtual_descriptor_t x;
        word_t raw;
    };
};

class phys_desc_t
{
public:
    inline word_t get_raw()
        { return raw; }

    inline void clear()
        { raw = 0; }

    inline u64_t get_base()
        { return ((u64_t)x.base) << __L4_MAP_SHIFT; }

    inline void set_base(u64_t base)
        { x.base = base >> __L4_MAP_SHIFT; }

    inline word_t get_attributes()
        { return x.attr; }

    inline void set_attributes(word_t attr)
        { x.attr = attr; }

    inline void set_raw(word_t val)
        { this->raw = val; }
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

class perm_desc_t
{
public:
    inline word_t get_raw()
        { return raw; }

    inline void clear()
        { raw = 0; }

    inline void set_perms(word_t rwx)
        { x.rwx = rwx; }

    inline void set_reference(word_t RWX)
        { x.RWX = RWX; }

    inline word_t get_reference()
        { return x.RWX; }

    inline void set_size(word_t size)
        { x.size = size; }

    inline void set_raw(word_t val)
        { this->raw = val; }
private:
    union {
        struct {
            BITFIELD4(word_t,
                    rwx         : 4,
                    size        : 6,
                    __res       : BITS_WORD - 14,
                    RWX         : 4);
        } x;
        word_t raw;
    };
};

class map_control_t
{
public:
    inline bool is_modify()
        { return x.m == 1; }

    inline bool is_query()
        { return x.q == 1; }

#if defined(CONFIG_ARM_V5)
    inline bool is_window()
        { return x.win == 1; }
#endif

    inline word_t highest_item()
        { return x.n; }

    inline void set_raw(word_t val)
        { this->raw = val; }
private:
    union {
        struct {
#if defined(CONFIG_ARM_V5)
            BITFIELD5(word_t,
                    n           : 6,
                    __res       : BITS_WORD - 9,
                    win         : 1,
                    q           : 1,
                    m           : 1);
#else
            BITFIELD5(word_t,
                    n           : 6,
                    __res       : BITS_WORD - 9,
                                : 1,
                    q           : 1,
                    m           : 1);
#endif
        } x;
        word_t raw;
    };
};

#endif /* __MAP_H__ */
