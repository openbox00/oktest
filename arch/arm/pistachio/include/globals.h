/*
 * Description:   Global objects/pointers
 */

#ifndef __ARM__GLOBALS_H__
#define __ARM__GLOBALS_H__

#define ARM_GLOBAL_BASE         VIRT_ADDR_RAM

#if !defined(ASSEMBLY)

class tcb_t;
class space_t;
class pgent_t;
class arm_fass_t;
class clist_t;

typedef struct {
    tcb_t * current_tcb;
    clist_t * current_clist;
    space_t * kernel_space;
    pgent_t * cpd;
    word_t current_domain;
    word_t current_domain_mask;
    word_t domain_dirty;
    arm_fass_t *arm_fass;
    tcb_t * current_schedule;    /* Physical address of kernel */
    word_t phys_addr_ram;
} arm_globals_t;

arm_globals_t * get_arm_globals() PURE;

INLINE arm_globals_t* get_arm_globals()
{
    return (arm_globals_t*)ARM_GLOBAL_BASE;
}

#endif

#endif /*__ARM__GLOBALS_H__*/

