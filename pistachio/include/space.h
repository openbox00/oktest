/*
 * Description:
 */
#ifndef __SPACE_H__
#define __SPACE_H__

#include <debug.h>
#include <fpage.h>
#include <capid.h>
#include <queueing.h>
#include <map.h>
#include <spaceid.h>
#include <clist.h>
#include <resource_ids.h>
#include <arch/pgent.h>
#include <phys_segment.h>

class utcb_t;
class tcb_t;
class space_t;
class kmem_resource_t;

class space_perm_t
{
public:
    union {
        struct {
            BITFIELD2( word_t,
                       plat_control : 1,
                       __res        : BITS_WORD - 1);
        } access;
        u16_t raw;
    };
};

class generic_space_t
{
public:
    enum access_e {
        read            = ARCH_READ,
        write           = ARCH_WRITE,
        readwrite       = ARCH_READWRITE,
        execute         = ARCH_EXECUTE
    };

    /* initialize and free */
    bool init(fpage_t utcb_area, kmem_resource_t *kresource);
    void free(kmem_resource_t *kresource);
    void arch_free (kmem_resource_t *kresource);

    bool check_utcb_location (word_t utcb_address);

    void enqueue_spaces();
    void dequeue_spaces();

    /* Allocate and free page directory */
    bool allocate_page_directory(kmem_resource_t *kresource);
    void free_page_directory(kmem_resource_t *kresource);

    /* page fault handling */
    void handle_pagefault(addr_t addr, addr_t ip, access_e access,
            bool kernel, continuation_t continuation);

    /* mapping */
    bool map_fpage(phys_desc_t base, fpage_t dest_fp, kmem_resource_t *kresource);
    void unmap_fpage(fpage_t fpage, bool unmap_all, kmem_resource_t *kresource);
    void read_fpage(fpage_t fpage, phys_desc_t *phys, perm_desc_t *perm);
    bool lookup_mapping (addr_t vaddr, pgent_t ** r_pg, pgent_t::pgsize_e * r_size);

    /* tcb management */
    void add_tcb(tcb_t * tcb);
    void remove_tcb(tcb_t * tcb);

    /* cap management */

    /* utcb_management */
    utcb_t *  allocate_utcb(tcb_t * tcb, kmem_resource_t *kresource);
    void free_utcb(utcb_t * utcb);
    void free_utcb_area_memory();

    /* kernel space management */
    void init_kernel_mappings();
    void init_cpu_mappings(scheduler_domain_t cpu);
    void free_utcb_page(pgent_t * pg, pgent_t::pgsize_e pgsize, addr_t vaddr);
    bool sync_kernel_space(addr_t addr);


    /* address ranges */
    bool is_user_area(addr_t addr);
    bool is_user_area(fpage_t fpage);
    bool is_utcb_area(addr_t addr);
    bool contains_utcb_area(addr_t addr, word_t size);
    bool is_mappable(addr_t addr);
    bool is_mappable(addr_t start, addr_t end);
    bool is_mappable(fpage_t fpage);

    /* security */
    void init_security(clist_t* new_clist)
        {
            clist = new_clist;
            clist->add_space((space_t*)this);

            permissions.raw = 0;
        }
    void free_security(void);

    void allow_plat_control(void) { permissions.access.plat_control = 1; }
    void restrict_plat_control(void) { permissions.access.plat_control = 0; }
    bool may_plat_control(void) { return permissions.access.plat_control == 1; }

    clist_t * lookup_clist(clistid_t clist_id); 
    space_t * lookup_space(spaceid_t space_id);
    //mutex_t * lookup_mutex(mutexid_t mutex_id);

    /* temporary */
    bool can_access_kresources(void) { return kmem_resource != NULL; }
    void grant_access_to_kresources(void);

    /* enable the space */
    void activate(tcb_t *tcb);

    /* accessor functions */
    ringlist_t<space_t> get_spaces_list() { return spaces_list; }
    tcb_t * get_thread_list() { return thread_list; }
    spaceid_t get_space_id() { return space_id; }
    clist_t* get_clist() { return clist; }
    fpage_t get_utcb_area (void) { return utcb_area; }
    word_t get_thread_count (void) { return thread_count; }
    prio_t get_maximum_priority(void) { return maximum_priority; }

    //void set_space_id(spaceid_t id) { space_id = id; }
    void set_utcb_area (fpage_t f) { utcb_area = f; }
    void set_thread_count (word_t c) { thread_count = c; }
    void set_maximum_priority(prio_t prio) { maximum_priority = prio; }

    void set_kmem_resource(kmem_resource_t * res) { kmem_resource = res; }
    kmem_resource_t * get_kmem_resource(void) { return kmem_resource; }

    /* read memory */
    bool readmem (addr_t vaddr, word_t * contents);
    static word_t readmem_phys (addr_t vaddr, addr_t paddr);

    /* tlb flushing */
    void flush_tlb (space_t * curspace);
    void flush_tlbent_local (space_t * curspace, addr_t vaddr, word_t log2size);
    bool does_tlbflush_pay (word_t log2size);

    /* update hooks */
    static void begin_update() { }
    static void end_update() { }

    /* generic page table walker */
    pgent_t * pgent (word_t num, word_t cpu = 0);

    /* static functions used at init time only */
    static void set_kernel_page_directory(pgent_t * pdir);

    /* space_t data  - layed out for cache locality */
    /* Note that this structure is not always cache aligned */

private:
    /* uncommonly accessed data */
    word_t              thread_count;
    ringlist_t<space_t> spaces_list;
    tcb_t *             thread_list;

private:
    /* moderate use data */
    fpage_t             utcb_area;
    spaceid_t           switch_space;
    spaceid_t           space_id;
    clist_t *           clist;
    space_perm_t        permissions;
    prio_t              maximum_priority;

private:
    /* commonly accessed data */
#if defined (CONFIG_CPULOCAL_PDIRS)
    pgent_t *           pdir[CONFIG_NUM_DOMAINS];
#else
    pgent_t *           pdir;
#endif
public:
    segment_list_t *    phys_segment_list;
private:
    kmem_resource_t *   kmem_resource;
public:
    resource_ids_t      mutex_range;
    resource_ids_t      space_range;
    resource_ids_t      clist_range;

#ifdef CONFIG_SPACE_NAMES
    char                debug_name[MAX_DEBUG_NAME_LENGTH];
#endif

    /* arch specific data ends up here */

    /* allow asmsyms here */
    friend void mkasmsym();
};

/* global functions  - should these be static functions? */
space_t * allocate_space(kmem_resource_t* res, spaceid_t, clist_t* clist);
void free_space(space_t * space);
void init_kernel_space();
void SECTION(SEC_INIT) setup_initial_mappings (space_t * space);

#include <arch/space.h>

/* Table containing mappings from spaceid_t to space_t* */
extern space_t * space_table;

/**
 * enqueue a space into the spaces list
 * These are now compiled in all the time so that ipc control ban bit
 * can be cleared when a space is destroyed.
 */
extern space_t * global_spaces_list;
extern spinlock_t spaces_list_lock;

INLINE void generic_space_t::enqueue_spaces()
{
    spaces_list_lock.lock();
    ENQUEUE_LIST_TAIL(space_t, global_spaces_list,
            ((space_t *)this), spaces_list);
    spaces_list_lock.unlock();
}

INLINE void generic_space_t::dequeue_spaces()
{
    spaces_list_lock.lock();
    DEQUEUE_LIST(space_t, global_spaces_list, this, spaces_list);
    spaces_list_lock.unlock();
}

INLINE bool is_kresourced_space(space_t * space)
{
    return space->can_access_kresources();
}

INLINE bool generic_space_t::is_utcb_area(addr_t addr)
{
    return get_utcb_area().is_addr_in_fpage(addr);
}

INLINE bool generic_space_t::contains_utcb_area(addr_t addr, word_t size)
{
    addr_t end = (addr_t)(((word_t)addr)+size);
    return get_utcb_area().is_range_overlapping(addr, end);
}

INLINE bool generic_space_t::is_mappable(addr_t addr)
{
    return (is_user_area(addr)
#if (CONFIG_UTCB_SIZE != 0)     /* UTCB is user allocated */
            && (!get_utcb_area().is_addr_in_fpage(addr))
#endif
           );
}

INLINE bool generic_space_t::is_mappable(addr_t start, addr_t end)
{
    return (this->is_user_area(start) && this->is_user_area((addr_t)((word_t)end-1))
#if (CONFIG_UTCB_SIZE != 0)     /* UTCB is user allocated */
            && (!this->get_utcb_area ().is_range_overlapping (start, end))
#endif
           );
}

INLINE bool generic_space_t::is_mappable(fpage_t fpage)
{
    return (this->is_user_area(fpage)
#ifndef CONFIG_ARCH_ARM
            && (!get_utcb_area().is_overlapping(fpage))
#endif
           );
}

INLINE bool generic_space_t::is_user_area(fpage_t fpage)
{
    return is_user_area(fpage.get_address()) &&
        is_user_area(addr_offset(fpage.get_address(), fpage.get_size()-1));
}

INLINE void generic_space_t::free_security(void)
{
    clist_t *list = clist;
    clist = NULL;
    list->remove_space((space_t*)this);
}


INLINE spaceid_t spaceid(word_t id)
{
    spaceid_t t;
    t.set_raw(id);
    return t;
}

/* May as well return kernel space since mappings should be identical for
 * the kernel area as in the each of the user's address spaces.
 */

#ifndef ARCH_ARM
/* ARM uses software GOT for this now */
INLINE space_t * get_kernel_space()
{
    extern space_t __kernel_space_object;
    return &__kernel_space_object;
}
#endif

INLINE void generic_space_t::set_kernel_page_directory(pgent_t * pdir)
{
    get_kernel_space()->pdir = pdir;
}

INLINE void generic_space_t::grant_access_to_kresources(void)
{
    set_kmem_resource(get_current_kmem_resource());
}

#endif /* !__SPACE_H__ */
