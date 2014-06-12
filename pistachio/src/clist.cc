/*
 * Description:   thread capability list
 */
#include <tcb.h>
#include <clist.h>

/* Table containing mappings from clistid_t to clist_t* */
clistid_lookup_t clist_lookup;

void SECTION(SEC_INIT) init_clistids(word_t max_clistids,
                                     kmem_resource_t *kresource)
{
    void * new_table;
    new_table = kresource->alloc(kmem_group_clistids, sizeof(clist_t*) * max_clistids, true);
    clist_lookup.init(new_table, max_clistids);
}

clist_t *create_clist(kmem_resource_t *res, word_t id, word_t max_caps)
{
    clist_t *clist;
    return clist;
}

tcb_t* clist_t::lookup_thread_cap_locked(capid_t tid, bool write, tcb_t *tcb_locked)
{
    tcb_t* tcb;
    return tcb;
}

tcb_t* clist_t::lookup_thread_cap_unlocked(capid_t tid)
{
    tcb_t* tcb;
    return tcb;
}

tcb_t* clist_t::lookup_ipc_cap_locked(capid_t tid, tcb_t *tcb_locked)
{
    tcb_t* tcb;
    return tcb;
}

cap_t* clist_t::lookup_cap(capid_t tid)
{
    /* Thread ID appears valid. Get the entry. */
    cap_t *entry = &this->entries[tid.get_index()];
    return entry;
}

bool clist_t::add_thread_cap(capid_t tid, tcb_t *tcb)
{
    return true;
}

bool clist_t::add_ipc_cap(capid_t tid, tcb_t *tcb)
{
    return true;
}

bool clist_t::remove_thread_cap(capid_t tid)
{
    return true;
}

bool clist_t::remove_ipc_cap(capid_t tid)
{
    return true;
}

bool clist_t::is_empty(void)
{
    return false;
}


