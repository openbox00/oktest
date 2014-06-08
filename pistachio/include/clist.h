/*
 * Description: Capability list
 */
#ifndef __CLIST_H__
#define __CLIST_H__

#include <debug.h>
#include <caps.h>
#include <sync.h>
#include <capid.h>
#include <idtable.h>
#if defined(CONFIG_ARCH_ARM)
#include <arch/globals.h>
#endif

class tcb_t;
class clist_t;
class kmem_resource_t;

class clistid_t
{
public:
    word_t get_clistno() const { return this->clistno; }

    word_t get_raw() { return this->raw; }
    void set_raw(word_t raw_) { this->raw = raw_; }
private:
    union {
        word_t raw;
        word_t clistno;
    };
};

INLINE clistid_t clistid(word_t rawid)
{
    clistid_t c;
    c.set_raw(rawid);
    return c;
}

/*
 * The Capability List Class
 */
class clist_t
{
public:
    /**
     * Lookup a thread cap in the clist
     *
     * @param tid   Thread-id of thread being looked up.
     * @param write   True if write lock required, false for read lock.
     * @returns TCB if lookup succeeded, NULL if thread not found.
     */
    tcb_t* lookup_thread_cap_locked(capid_t tid, bool write = false, tcb_t *tcb_locked = NULL);

    /**
     * Lookup a thread cap in the clist
     *
     * @param tid   Thread-id of thread being looked up.
     * @returns TCB if lookup succeeded, NULL if thread not found.
     */
    tcb_t* lookup_thread_cap_unlocked(capid_t tid);

    /**
     * Lookup a ipc cap in the clist, and get read-lock
     *
     * @param tid   Thread-id of thread being looked up.
     * @returns TCB if lookup succeeded, NULL if thread not found.
     */
    tcb_t* lookup_ipc_cap_locked(capid_t tid, tcb_t *tcb_locked = NULL);

    /**
     * Lookup a cap in the clist, not locked
     *
     * @param tid   Thread-id of thread being looked up.
     * @returns CAP if lookup succeeded, NULL if thread not found.
     */
    cap_t* lookup_cap(capid_t tid);

    /**
     * Check if cap number is valid in this clist
     *
     * @param tid   Thread-id of thread being looked up.
     * @return true if thread-id in range, false if not in range
     */
    bool is_valid(capid_t tid) {
        return (tid.get_index() > max_id) ? false : true;
    }

    /**
     * Add a thread cap to the clist.
     *
     * @pre is_valid(tid)
     *
     * @returns true if cap was added, false if cap slot not available.
     */
    bool add_thread_cap(capid_t tid, tcb_t *tcb);

    /**
     * Remove a thread cap from the clist
     *
     * @pre is_valid(tid), thread locked
     *
     * @returns true if cap was removed, false if cap not found.
     */
    bool remove_thread_cap(capid_t tid);

    /**
     * Add a ipc cap to the clist
     *
     * @pre is_valid(tid)
     *
     * @returns true if cap was added, false if cap slot not available.
     */
    bool add_ipc_cap(capid_t tid, tcb_t *tcb);

    /**
     * Remove a ipc cap from the clist
     *
     * @pre lookup_ipc_cap()
     *
     * @returns true if cap was removed, false if cap not found.
     */
    bool remove_ipc_cap(capid_t tid);

    /**
     * Initialize clist
     */
    void init(word_t num_ids) {
        ASSERT(DEBUG, num_ids > 0);
        max_id = num_ids - 1;
        num_spaces = 0;
        list_lock.init();
    }

    /**
     * Get size of clist
     */
    word_t num_entries(void) {
        return max_id + 1;
    }

    /**
     * Add an address space as using this clist
     */
    void add_space(space_t *space) { num_spaces ++; };

    /**
     * Remove an address space using this clist
     */
    void remove_space(space_t *space) { num_spaces --; };

    /**
     * Get number of spaces using clist
     */
    word_t get_space_count(void) {
        return num_spaces;
    }

    /**
     * Check if any caps remaining in the clist
     *
     * @returns true if clist empty, false if clist not empty
     */
    bool is_empty(void);

private:
    friend void mkasmsym();

    word_t max_id;
    word_t num_spaces;
    spinlock_t list_lock;

    /*lint -e1501 */
    cap_t entries[0];
};

/*
 * The Capability List lookup table
 */
class clistid_lookup_t : private id_lookup_t
{
public:
    bool is_valid(clistid_t id) {
        return valid_id(id.get_clistno());
    }

    clist_t * lookup_clist(clistid_t id) {
        return (clist_t*)lookup_id(id.get_clistno());
    }

public:
    void add_clist(clistid_t id, clist_t* clist) {
        add_object(id.get_clistno(), clist);
    }

    void remove_clist(clistid_t id) {
        remove_object(id.get_clistno());
    }
private:
    friend void init_clistids(word_t max_clistids, kmem_resource_t *kresource);
};

/* Access functions */

INLINE clistid_lookup_t *
get_clist_list()
{
    extern clistid_lookup_t clist_lookup;
    return &clist_lookup;
}

extern void init_clistids(word_t max_clistids, kmem_resource_t *kresource);

#endif /* !__CLIST_H__ */
