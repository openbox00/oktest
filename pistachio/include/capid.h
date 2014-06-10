/*
 * Description: Capability Identifiers
 */
#ifndef __CAPID_H__
#define __CAPID_H__

#if defined(L4_32BIT)
#define ANYTHREAD_RAW       0xffffffffUL
#define WAITNOTIFY_RAW      0xfffffffeUL
#define MYSELF_RAW          0xfffffffdUL
#define NILTHREAD_RAW       0xfffffffcUL
#define SPECIAL_RAW_LIMIT   0xfffffffbUL
#define INVALID_RAW         0x0fffffffUL

#elif defined(L4_64BIT)
#define ANYTHREAD_RAW       0xffffffffffffffffUL
#define WAITNOTIFY_RAW      0xfffffffffffffffeUL
#define MYSELF_RAW          0xfffffffffffffffdUL
#define NILTHREAD_RAW       0xfffffffffffffffcUL
#define SPECIAL_RAW_LIMIT   0xfffffffffffffffbUL
#define INVALID_RAW         0x0fffffffffffffffUL

#else
#error Unknown wordsize
#endif

#define TYPE_CAP            0x0
#define TYPE_TCB_RESOURCE   0x1
#define TYPE_MUTEX_RESOURCE 0x2
#define TYPE_SPACE_RESOURCE 0x3
#define TYPE_CLIST_RESOURCE 0x4
#define TYPE_PHYS_MEM_SEG   0x5
#define TYPE_THREAD_HANDLE  0x8
#define TYPE_SPECIAL        0xf

#define CAPID_INDEX_MASK ((1UL << (BITS_WORD - 4)) - 1)

class capid_t
{
public:
    static capid_t nilthread() {
        capid_t cid;
        cid.raw = NILTHREAD_RAW;
        return cid;
    }

    static capid_t anythread() {
        capid_t cid;
        cid.raw = ANYTHREAD_RAW;
        return cid;
    };

    static capid_t waitnotify() {
        capid_t cid;
        cid.raw = WAITNOTIFY_RAW;
        return cid;
    };

    static capid_t myselfthread() {
        capid_t cid;
        cid.raw = MYSELF_RAW;
        return cid;
    };

    static capid_t capid(word_t type, word_t index) {
        capid_t cid;
        cid.raw = 0;
        cid.id.index = index;
        cid.id.type = type;
        return cid;
    }

    /* check for specific (well known) thread ids */
    bool is_nilthread() { return this->raw == NILTHREAD_RAW; }
    bool is_anythread() { return this->raw == ANYTHREAD_RAW; }
    bool is_waitnotify(){ return this->raw == WAITNOTIFY_RAW; }
    bool is_myself()    { return this->raw == MYSELF_RAW; }

    bool is_threadhandle()
        {
            return this->id.type == TYPE_THREAD_HANDLE;
        }
    bool is_cap_type()
        {
            return id.type == TYPE_CAP;
        }

    word_t get_index() { return id.index; }
    word_t get_type() { return id.type; }

    word_t get_raw() { return this->raw; }
    void set_raw(word_t raw) { this->raw = raw; }

    /* operators */
    bool operator == (const capid_t & cid) const
        {
            return this->raw == cid.raw;
        }

    bool operator != (const capid_t & cid) const
        {
            return this->raw != cid.raw;
        }

private:
    union {
        word_t raw;

        struct {
            BITFIELD2(word_t,
                      index : BITS_WORD-4,
                      type  : 4);
        } id;
    };
};

INLINE capid_t capid(word_t rawid)
{
    capid_t t;
    t.set_raw (rawid);
    return t;
}

INLINE capid_t threadhandle(word_t handle)
{
    capid_t t;
    t = capid_t::capid(TYPE_THREAD_HANDLE, handle);
    return t;
}

/* special ids */
#define NILTHREAD       (capid_t::nilthread())
#define ANYTHREAD       (capid_t::anythread())
#define WAITNOTIFY      (capid_t::waitnotify())

#endif /* !__CAPID_H__ */

