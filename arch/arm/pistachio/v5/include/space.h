/*
 * Description: ARMv5 specific space implementation.
 */

#ifndef __GLUE__V4_ARM__V5__SPACE_H
#define __GLUE__V4_ARM__V5__SPACE_H

#include <l4/arch/ver/space_resources.h>


class space_t : public generic_space_t
{
public:
    enum rwx_e {
        read_only       = 0x1,
        read_write      = 0x3,
        read_execute    = read_only,
        read_write_ex   = read_write,
    };

    arm_domain_t get_domain(void);
    word_t get_domain_mask(void);
    void set_domain(arm_domain_t new_domain);

    /* Sharing another address space's domain */
    bool add_shared_domain(space_t *space, bool manager);
    bool remove_shared_domain(space_t *space);
    bool is_client_space(space_t *space);

    void add_domain_access(arm_domain_t domain, bool manager);
private:
    void remove_domain_access(arm_domain_t domain);

public:
    /* domain sharing */
    bool is_sharing_domain(space_t *space);
    bool is_manager_of_domain(space_t *target);
    //bool domain_link(generic_space_t *source);
    bool domain_unlink(generic_space_t *source);
    bool window_share_fpage(space_t *space, fpage_t fpage,
                            kmem_resource_t *kresource);

    /* Manage access to this space's domain */
    void flush_sharing_spaces(void);
    void flush_sharing_on_delete(void);

    /* PID management */
    arm_pid_t get_pid(void);
    void set_pid(arm_pid_t new_pid);

    word_t get_vspace(void);
    void set_vspace(word_t vspace);

    bool is_domain_area (addr_t addr);

    word_t space_control (word_t ctrl);
    word_t space_control_window (word_t ctrl);

    pgent_t * pgent_utcb();

    bool add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                     rwx_e rwx, bool kernel, memattrib_e attrib,
                     kmem_resource_t *kresource);
    bool add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                     rwx_e rwx, bool kernel, kmem_resource_t *kresource)
    {
        return add_mapping(vaddr, paddr, size, rwx, kernel, cached, kresource);
    }

    bitmap_t* get_utcb_bitmap(void)
        {
            return &this->utcb_bitmap[0];
        }

    bitmap_t* get_client_spaces_bitmap(void)
        {
            return &this->client_spaces[0];
        }

    bitmap_t* get_shared_spaces_bitmap(void)
        {
            return &this->shared_spaces[0];
        }

    bitmap_t* get_manager_spaces_bitmap(void)
        {
            return &this->manager_spaces[0];
        }

private:
    word_t domain;
    word_t domain_mask;
public:
    union {
        struct {
            BITFIELD4 (word_t,
                    __zero      : 2,
                    pid : 7,
                    __fill      : 7,
                    vspace      : 16
                    );
        } bits;
        word_t      pid_vspace_raw;
    };
    pgent_t utcb_pgent;

    /*tcb_t * threads;*/
    bitmap_t utcb_bitmap[BITMAP_SIZE(UTCB_BITMAP_LENGTH)] ;

    /* spaces sharing our domain */
    bitmap_t client_spaces[BITMAP_SIZE(CONFIG_MAX_SPACES)];
    /* shared spaces - domains we have access to */
    bitmap_t shared_spaces[BITMAP_SIZE(CONFIG_MAX_SPACES)];
    /* manager access - domains we have manager access to (RW regardless of page rights) */
    bitmap_t manager_spaces[BITMAP_SIZE(CONFIG_MAX_SPACES)];

    friend void mkasmsym(void);
};

/**
 * Return pointer to ARMv5 UTCB Level1 pageatable entry
 */
INLINE pgent_t *space_t::pgent_utcb()
{
    return &(this->utcb_pgent);
}

/**
 * Get pointer to ARMv5 caching page directory (CPD)
 */
INLINE pgent_t * get_cpd() PURE;

INLINE pgent_t * get_cpd()
{
    return get_arm_globals()->cpd;
}

/**
 * Ensure the CPD is in sync with the updated pagetable
 */
INLINE void pgent_t::cpd_sync (generic_space_t * s)
{
    arm_domain_t n = ((space_t *)s)->get_domain();
    /* complicated offset calculation so only bother if we have valid domain */
    if (n && (n != INVALID_DOMAIN)) {
        word_t offset = ARM_HWL1_SIZE;
        /* special case for utcb_section */
        if (this == ((space_t*)s)->pgent_utcb()) {
            offset = ( UTCB_AREA_START/ARM_SECTION_SIZE +
                        (s->get_space_id().get_spaceno()) ) * sizeof(pgent_t);
        } else {
            /* find which level 1 page table it is in */
            for (word_t i = 0; i < ARM_L0_ENTRIES; i++) {
                pgent_t * pg = s->pgent(i);
                if (pg->is_valid(s, pgent_t::size_level0) && pg->is_subtree(s, pgent_t::size_level0)) {
                    offset = ((word_t) this) - ((word_t)pg->subtree(s, pgent_t::size_level0));
                    if (offset < ARM_L1_SIZE) {
                        offset += i*ARM_L1_SIZE;
                        break;
                    }
                }
            }
        }

        /* make sure pgent is in space */
        pgent_t *cpd_entry = (pgent_t *)(((word_t)get_cpd()) + offset);

        if ((cpd_entry->get_domain() == n)) {
            pgent_t temp;
            temp.raw = this->raw;
            temp.set_domain(n);
            cpd_entry->raw = temp.raw;
        }
    }
}

/**
 * Virtual space for single address space support
 */
INLINE word_t space_t::get_vspace(void)
{
    return this->bits.vspace;
}

/**
 * ARMv5 specific part of space_control()
 */

#define SPACE_CONTROL_WINDOW_MASK  L4_SPACE_RESOURCES_WINDOW_REVOKE

INLINE word_t space_t::space_control (word_t ctrl)
{
    set_pid(ctrl & PID_MASK);
    set_vspace(ctrl >> 16);
    if (ctrl & SPACE_CONTROL_WINDOW_MASK) {
        return space_control_window(ctrl & SPACE_CONTROL_WINDOW_MASK);
    }
    return 0;
}

/* Domain Fault Area */
INLINE bool space_t::is_domain_area (addr_t addr)
{
    return ((word_t)addr < UTCB_AREA_END);
}

INLINE arm_domain_t space_t::get_domain(void)
{
    return this->domain;
}

INLINE void space_t::set_domain(arm_domain_t new_domain)
{
    if (new_domain != INVALID_DOMAIN) {
        this->domain_mask = (1UL << (new_domain*2)) | 1;
    } else {
        //printf("unshare %p : %d\n", this, get_domain());
        this->domain_mask = 0;
    }
    this->domain = new_domain;
}

INLINE word_t space_t::get_domain_mask(void)
{
    return this->domain_mask;
}

INLINE void space_t::add_domain_access(arm_domain_t domain, bool manager)
{
    this->domain_mask |= ((manager ? 3UL : 1UL) << (domain*2));
}

INLINE void space_t::remove_domain_access(arm_domain_t domain)
{
    /*
     * Clear our domain_mask since we will cache_flush soon
     * and we don't want to mark clean domains as dirty
     */
    if (this->domain != INVALID_DOMAIN)
        this->domain_mask = (1UL << (this->domain*2)) | 1;
    else
        this->domain_mask = 1;
}

INLINE arm_pid_t space_t::get_pid(void)
{
    return this->bits.pid;
}

INLINE void space_t::set_pid(arm_pid_t new_pid)
{
    this->bits.pid = new_pid;
}

/**
 * Try to copy a mapping from kernel space into the current space
 * @param addr the address for which the mapping should be copied
 * @return true if something was copied, false otherwise.
 * Synchronization must happen at the highest level, allowing sharing.
 */
INLINE bool generic_space_t::sync_kernel_space(addr_t addr)
{
    /* We set everything up at initialisation time */
    return false;
}

#endif /*__GLUE__V4_ARM__V5__SPACE_H*/
