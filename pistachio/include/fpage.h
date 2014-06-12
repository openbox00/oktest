/*
 * Description:   V4 flexpages
 */
#ifndef __API__V4__FPAGE_H__
#define __API__V4__FPAGE_H__

class fpage_t
{
    /* data members */
public:
    union {
        struct {
            BITFIELD7(word_t,
                execute         : 1,
                write           : 1,
                read            : 1,
                meta            : 1,    /* extended attributes */
                size            : 6,
                base            : L4_FPAGE_BASE_BITS,
                                : (BITS_WORD - L4_FPAGE_BASE_BITS - 10)
                );
        } x;
        word_t raw;
    };

public:
    void set(word_t base, word_t size, bool read, bool write, bool exec)
        {
            raw = 0;
            x.base = (base & (~0UL << size)) >> 10;
            x.size = size;
            x.read = read;
            x.write = write;
            x.execute = exec;
        }

    bool is_complete_fpage()
        { return (x.size == 1) && (x.base == 0); }

    bool is_range_in_fpage(addr_t start, addr_t end)
        {
            /* VU: should we make sure, that base is valid? */
            return is_complete_fpage () ||
                (get_address() <= start &&
                 addr_offset(get_address(), get_size()) >= end);
        }

    bool is_addr_in_fpage(addr_t addr)
        { return is_range_in_fpage (addr, (addr_t)((word_t)addr + sizeof(addr_t))); }

    addr_t get_address()
        { return (addr_t)(x.base << 10 & ~((~0UL) >> ((sizeof(word_t)*8) - x.size))); }

    word_t get_size()
        { return 1UL << x.size; }

    word_t get_size_log2()
        { return is_complete_fpage() ? sizeof(word_t) * 8 : x.size; }

    bool is_read()
        { return x.read; }

    bool is_write()
        { return x.write; }

    bool is_execute()
        { return x.execute; }

    void set_rwx()
        { x.read = 1; x.write = 1; x.execute = 1; }

    void set_rwx(word_t rwx)
        { raw |= (rwx & 7); }
};

#endif /* !__API__V4__FPAGE_H__ */
