/*
 * Description:   thread capability list
 */
#include <l4.h>
#include <tcb.h>
#include <clist.h>
#include <init.h>
#include <ref_inline.h>


GLOBAL_DEF(clist_t *current_clist = NULL)

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

    clist = (clist_t*)res->alloc(kmem_group_root_clist,
            sizeof(clist_t) + (max_caps * sizeof(cap_t)), true);
    if (!clist) {
        return NULL;
    }
    clist->init(max_caps);
    get_clist_list()->add_clist(clistid(id), clist);

    return clist;
}

tcb_t* clist_t::lookup_thread_cap_locked(capid_t tid, bool write, tcb_t *tcb_locked)
{
    tcb_t* tcb;

    if (EXPECT_FALSE(tid.get_type() != TYPE_CAP)) {
        return NULL;
    }

    /* Check cap-id range, 0 is a valid cap-id */
    if (EXPECT_FALSE(tid.get_index() > max_id))
    {
        return NULL;
    }

again:
    okl4_atomic_barrier_smp();
    /* Thread ID appears valid. Get the entry. */
    cap_t entry = this->entries[tid.get_index()];

    if (!entry.is_thread_cap()) {
        return NULL;
    }
    tcb = entry.get_tcb();

    if (EXPECT_FALSE(write)) {
        if (EXPECT_FALSE(!tcb->try_lock_write())) {
            okl4_atomic_barrier_smp();
            goto again;
        }
    } else {
        if (tcb == tcb_locked) {
            //tcb->lock_read_already_held();
        } else if (EXPECT_FALSE(!tcb->try_lock_read())) {
            okl4_atomic_barrier_smp();
            goto again;
        }
    }
    return tcb;
}

tcb_t* clist_t::lookup_thread_cap_unlocked(capid_t tid)
{
    tcb_t* tcb;

    if (EXPECT_FALSE(tid.get_type() != TYPE_CAP)) {
        return NULL;
    }

    /* Check cap-id range, 0 is a valid cap-id */
    if (EXPECT_FALSE(tid.get_index() > max_id))
    {
        return NULL;
    }

    /* Thread ID appears valid. Get the entry. */
    cap_t entry = this->entries[tid.get_index()];

    if (!entry.is_thread_cap()) {
        return NULL;
    }

    tcb = entry.get_tcb();
    return tcb;
}

tcb_t* clist_t::lookup_ipc_cap_locked(capid_t tid, tcb_t *tcb_locked)
{
    tcb_t* tcb;

    if (EXPECT_FALSE(tid.get_type() != TYPE_CAP)) {
        return NULL;
    }

    /* Check cap-id range, 0 is a valid cap-id */
    if (EXPECT_FALSE((tid.get_index() > max_id)))
    {
        return NULL;
    }

again:
    //okl4_atomic_barrier_smp();
    /* Thread ID appears valid. Get the entry. */
    cap_t entry = this->entries[tid.get_index()];

    if (!entry.can_ipc_cap()) {
        return NULL;
    }
    tcb = entry.get_tcb();

    if (tcb == tcb_locked) {
        //tcb->lock_read_already_held();
    } else if (EXPECT_FALSE(!tcb->try_lock_read())) {
        //okl4_atomic_barrier_smp();
        goto again;
    }
    return tcb;
}

cap_t* clist_t::lookup_cap(capid_t tid)
{
    if (EXPECT_FALSE(tid.get_type() != TYPE_CAP)) {
        return NULL;
    }

    /* Check cap-id range, 0 is a valid cap-id */
    if (EXPECT_FALSE((tid.get_index() > max_id)))
    {
        return NULL;
    }

    /* Thread ID appears valid. Get the entry. */
    cap_t *entry = &this->entries[tid.get_index()];

    if (!entry->is_valid_cap()) {
        return NULL;
    }
    return entry;
}

bool clist_t::add_thread_cap(capid_t tid, tcb_t *tcb)
{
    list_lock.lock();
    cap_t *entry = &this->entries[tid.get_index()];

    if (EXPECT_FALSE(entry->is_valid_cap())) {
        list_lock.unlock();
        return false;
    }

    entry->set_thread_cap(tcb);
    list_lock.unlock();

    return true;
}

bool clist_t::add_ipc_cap(capid_t tid, tcb_t *tcb)
{
    list_lock.lock();
    cap_t *entry = &this->entries[tid.get_index()];

    if (EXPECT_FALSE(entry->is_valid_cap())) {
        list_lock.unlock();
        return false;
    }

    entry->set_ipc_cap(tcb);
    list_lock.unlock();

    return true;
}

bool clist_t::remove_thread_cap(capid_t tid)
{
    list_lock.lock();
    /* Remove the entry. */
    cap_t entry = this->entries[tid.get_index()];

    if (EXPECT_FALSE(!entry.is_thread_cap())) {
        list_lock.unlock();
        return false;
    }

    list_lock.unlock();
    return true;
}

bool clist_t::remove_ipc_cap(capid_t tid)
{
    list_lock.lock();
again:
    //okl4_atomic_barrier_smp();
    /* Remove the entry. */
    cap_t entry = this->entries[tid.get_index()];

    if (EXPECT_FALSE(!entry.is_ipc_cap())) {
        list_lock.unlock();
        return false;
    }
    tcb_t *tcb = entry.get_tcb();
    if (EXPECT_FALSE(!tcb->try_lock_write())) {
        okl4_atomic_barrier_smp();
        goto again;
    }
    //tcb->unlock_write();

    list_lock.unlock();
    return true;
}

bool clist_t::is_empty(void)
{
    bool found = false;
    list_lock.lock();

    for (word_t i=0; i <= this->max_id; i++) {
        cap_t *entry = &this->entries[i];

        if (entry->is_valid_cap()) {
            found = true;
            break;
        }
    }

    list_lock.unlock();
    return (found == false);
}


