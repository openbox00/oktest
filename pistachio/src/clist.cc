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

