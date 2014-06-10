/*
 * Description:  Capability handling and abstraction
 */
#ifndef __CAPS_H__
#define __CAPS_H__


#include <read_write_lock.h>

#define CAP_OBJ_INDEX_MASK ((1UL << (BITS_WORD - 4)) - 1)

typedef enum
{
    cap_type_invalid,
    cap_type_thread,
    cap_type_ipc
} cap_type_e;

class cap_t;
class ref_t;
class tcb_t;

namespace cap_reference_t
{
    DECLARE_READ_WRITE_LOCK(cap_reference_lock);

    void add_reference(cap_t *master_cap, cap_t *cap);
    void remove_reference(cap_t *master_cap, cap_t *cap);
    void remove_reference(cap_t *master_cap, ref_t *ref);
    void invalidate_reference(cap_t *master_cap);
    void add_reference(tcb_t *tcb, cap_t *cap);
    void remove_reference(tcb_t *tcb, cap_t *cap);
    void invalidate_reference(tcb_t *tcb);
}

/**
 * Internal kernel revocable object
 * The next pointer must map to the cap_t structure
 */
class ref_t
{
private:
    friend void mkasmsym();
    void *object;

public:
    ref_t *next;

    /**
     * Initialize this reference
     */
    void init(void) {
        object = NULL;
        next = NULL;
    }
    /**
     * Get TCB pointed to by this reference
     */
    tcb_t* get_tcb(void) {
        return (tcb_t*)object;
    }
    /**
     * Set TCB pointed to by this reference
     */
    void set_referenced(tcb_t* obj);
    /**
     * Clear TCB reference
     */
    void remove_referenced(tcb_t* obj);
};

class cap_t
{
private:
    /* IMPORTANT: 
     * cap_t size MUST be aligned to 2 power N
     * in order to optimize access in fastpath,
     * use padding if it is not! Please also 
     * change LOG2_SIZEOF_CAP_T  when adding new members.
     */
    union {
        struct {
            BITFIELD2(word_t,
                    obj_idx : BITS_WORD - 4,
                    type    : 4
            );
        } x;
        word_t raw;
    };

public:
    cap_t * next;

    /**
     * Clear CAP entry
     */
    void clear(void) { this->raw = 0; }
    /**
     * Initialize/reset CAP entry
     */
    void init(void) { this->raw = 0; this->next = 0; }
    /**
     * Get TCB pointed to by this CAP
     */
    tcb_t* get_tcb(void) {
        return (tcb_t*)(ARCH_OBJECT_PTR(this->x.obj_idx));
    }

    /**
     * Make this a thread CAP
     */
    void set_thread_cap(const tcb_t *tcb) {
        cap_t cap;
        cap.x.type = (word_t)cap_type_thread;
        cap.x.obj_idx = ARCH_OBJECT_INDEX(tcb);
        this->raw = cap.raw;
    }
    /**
     * Make this an ipc CAP
     */
    void set_ipc_cap(const tcb_t *tcb) {
        cap_t cap;
        cap.x.type = (word_t)cap_type_ipc;
        cap.x.obj_idx = ARCH_OBJECT_INDEX(tcb);
        this->raw = cap.raw;
    }

private:
    /**
     * Check if we can ipc this cap
     */
    bool can_ipc_cap(void) { return (this->x.type == (word_t)cap_type_thread) || (this->x.type == (word_t)cap_type_ipc); };
    /**
     * Check if this is a valid
     */
    bool is_valid_cap(void) { return (this->raw != (word_t)cap_type_invalid); };
    /**
     * Check if this is a thread CAP
     */
    bool is_thread_cap(void) { return (this->x.type == (word_t)cap_type_thread); };
    /**
     * Check if this is an ipc CAP
     */
    bool is_ipc_cap(void) { return (this->x.type == (word_t)cap_type_ipc); };

    friend class clist_t;
    friend void mkasmsym();
    friend tcb_t * acquire_read_lock_tcb(tcb_t *, tcb_t *);
};

class cap_control_t
{
public:
    enum cap_op_e {
        op_create_clist = 0,
        op_delete_clist = 1,
        op_copy_cap     = 4,
        op_delete_cap   = 5
    };

    cap_op_e get_op() { return (cap_op_e)x.op; }

    word_t get_raw() { return raw; }

    void set_raw(word_t raw) { this->raw = raw; }

private:
    union {
        struct {
            BITFIELD2(word_t,
                    op      : 3,
                    _res1   : BITS_WORD - 3
            );
        } x;
        word_t raw;
    };
};

#endif /* __CAPS_H__ */
