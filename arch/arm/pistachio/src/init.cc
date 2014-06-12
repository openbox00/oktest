/*
 * Description:   ARM specific initialization code
 */
#include <schedule.h>
#include <queueing.h>
#include <linear_ptab.h>

/* Primary CP15 registers (CRn) */
#define C15_control             c1
#define C15_domain              c3


/* Default secondary register (CRm) */
#define C15_CRm_default         c0
/* Default opcode2 register (opcode2) */
#define C15_OP2_default         0

/* CP15 - Control Register */
#define C15_CONTROL_A           0x0002                  /* Alignment fault enable       */
#define C15_CONTROL_M           0x0001                  /* Memory management enable     */
#define C15_CONTROL_X           0x2000                  /* Remap interrupt vector       */

#define C15_CONTROL_INIT        0

#define C15_CONTROL_KERNEL      (C15_CONTROL_M | C15_CONTROL_X)

extern "C" {
    extern char _start_rom[];
    extern char _end_rom[];
    extern char _start_ram[];
    extern char _end_ram[];
    extern char _start_init[];
    extern char _end_init[];
    extern char _end[];
}



#define start_init              ((addr_t) _start_init)
#define end_init                ((addr_t) _end_init)


extern word_t arm_high_vector;

CONTINUATION_FUNCTION(idle_thread);

CONTINUATION_FUNCTION(idle_thread)
{
    while (1) {
    }
}

template<typename T> INLINE T virt_to_phys_init(T x, word_t *p)
{
        return (T) ((u32_t) x - VIRT_ADDR_RAM + *p);
}

template<typename T> INLINE T virt_to_page_table(T x)
{
    return (T) ((u32_t) x - VIRT_ADDR_RAM + VIRT_ADDR_PGTABLE);
}

INLINE void generic_space_t::set_kernel_page_directory(pgent_t * pdir)
{
    get_kernel_space()->pdir = pdir;
}

#define _INS_(x) #x
#define STR(x) _INS_(x)
#define _OUTPUT(x)  : [x] "=r" (x)
#define _INPUT(x)  :: [x] "r" (x)

#define read_cp15_register(CRn, CRm, op2, ret)                        \
    __asm__ __volatile__ (                                            \
        "mrc    p15, 0, "_(ret)","STR(CRn)", "STR(CRm)","STR(op2)";"  \
        _OUTPUT(ret))

#define write_cp15_register(CRn, CRm, op2, val)                     \
{                                                                   \
    word_t v = (word_t)val;                                         \
    __asm__ __volatile__ (                                          \
        "mcr    p15, 0, "_(v)","STR(CRn)", "STR(CRm)","STR(op2)";"  \
    _INPUT(v));                                                     \
};

INLINE addr_t addr_align (addr_t addr, word_t align)
{
    return addr_mask (addr, ~(align - 1));
}

extern word_t pre_tcb_init;

/* UTCB reference page. This needs to be revisited */
ALIGNED(UTCB_AREA_PAGESIZE) char arm_utcb_page[UTCB_AREA_PAGESIZE] UNIT("utcb");

prio_t
scheduler_t::get_highest_priority(void)
{
    word_t first_level_bitmap = prio_queue.index_bitmap;
    if (!first_level_bitmap) {
        return (prio_t)-1;
    }
    word_t first_level_index = msb(first_level_bitmap);
    word_t second_level_bitmap = prio_queue.prio_bitmap[first_level_index];
    word_t second_level_index = msb(second_level_bitmap);
    prio_t top_prio = first_level_index * BITS_WORD + second_level_index;
    return top_prio;
}
void
scheduler_t::schedule(tcb_t * current, continuation_t continuation,
                      flags_t flags)
{
    prio_t max_prio = get_highest_priority();
    bool current_runnable = current->get_state().is_runnable() && !current->is_reserved();
    if (current_runnable) {
        if (current->effective_prio > max_prio ||
                (!(flags & sched_round_robin)
                        && current->effective_prio == max_prio)) {
            schedule_lock.unlock();
            ACTIVATE_CONTINUATION(continuation);
        }
        if (flags & preempting_thread) {
            mark_thread_as_preempted(current);
        }
    }
    tcb_t *next;
    if (max_prio >= 0) {
        next = prio_queue.get(max_prio);
        dequeue(next);
    } else {
        next = get_idle_tcb();
    }
    (void)next->grab();
    switch_from(current, continuation);

	if (current_runnable && current != get_idle_tcb()) {
        enqueue(current);
    }
    switch_to(next, next);
}


extern "C" void SECTION(".init") init_arm_globals(word_t *physbase)
{
    extern char __kernel_space_object[];

    /* Copy physical address bases first */
    get_arm_globals()->phys_addr_ram = *physbase;

    get_arm_globals()->kernel_space = (space_t*)&__kernel_space_object;
    extern char _kernel_space_pagetable[];
    extern arm_fass_t arm_fass;

    get_arm_globals()->cpd = (pgent_t*)((word_t)(&_kernel_space_pagetable)
            - VIRT_ADDR_RAM + VIRT_ADDR_PGTABLE);
    get_arm_globals()->arm_fass = &arm_fass;
}

INLINE word_t page_table_1m (addr_t vaddr)
{
    return ((word_t) vaddr >> 20) & (ARM_HWL1_SIZE - 1);
}

extern "C" void SECTION(".init") add_mapping_init(pgent_t *pdir, addr_t vaddr,
                addr_t paddr, memattrib_e attrib)
{
    pgent_t *pg = pdir + (page_table_1m(vaddr));

    pg->set_entry_1m(NULL, paddr, true, true, true, attrib);
}

SECTION(".init")
void map_phys_memory(pgent_t *kspace_phys, word_t *physbase)
{
    for (word_t j = (word_t)VIRT_ADDR_BASE; j < (word_t)VIRT_ADDR_BASE + KERNEL_AREA_SIZE; j += PAGE_SIZE_1M)
        {
            word_t phys = virt_to_phys_init(j, physbase);
            add_mapping_init( kspace_phys, (addr_t)j, (addr_t)phys, writeback );
            add_mapping_init( kspace_phys, (addr_t)phys_to_page_table_init(phys, physbase), (addr_t)phys, writethrough );
        }
}


void SECTION(".init") init_arm_interrupts()
{
    bool r;
    r = get_kernel_space()->add_mapping((addr_t)ARM_HIGH_VECTOR_VADDR,
                                        (addr_t)virt_to_phys(&arm_high_vector),
                                        pgent_t::size_4k,
                                        space_t::read_write_ex,
                                        true,
                                        get_current_kmem_resource());    word_t control;
    
	read_cp15_register(C15_control, C15_CRm_default, C15_OP2_default, control);

    control |= C15_CONTROL_A;
    write_cp15_register(C15_control, C15_CRm_default, C15_OP2_default, control);

    /* Initialize cached values in VECTOR page */
    extern tcb_t** vector_tcb_handle_array, **thread_handle_array;
    extern word_t vector_num_tcb_handles, num_tcbs;

    /* Cache these values */
    vector_num_tcb_handles = num_tcbs;
    vector_tcb_handle_array = thread_handle_array;
}

extern bitmap_t ipc_bitmap_ids;


bool space_t::add_mapping(addr_t vaddr, addr_t paddr, pgent_t::pgsize_e size,
                          rwx_e rwx, bool kernel, memattrib_e attrib,
                          kmem_resource_t *kresource)
{
    pgent_t::pgsize_e pgsize = pgent_t::size_max;
    pgent_t *pg = this->pgent(0);
    pg = pg->next(this, pgsize, page_table_index(pgsize, vaddr));

    if (is_utcb_area(vaddr) && (this != get_kernel_space())) {
        /* Special case for UTCB mappings */
        pgsize = pgent_t::size_1m;
        pg = this->pgent_utcb();
    }
    /*
     * Lookup mapping
     */
    while (1) {
        if ( pgsize == size )
            break;

        if ( !pg->is_valid( this, pgsize ) ) {
            if (!pg->make_subtree( this, pgsize, kernel, kresource))
                return false;
        }

        pg = pg->subtree( this, pgsize )->next
            ( this, pgsize-1, page_table_index( pgsize-1, vaddr ) );
        pgsize--;
    }

    bool r = (word_t)rwx & 4;
    bool w = (word_t)rwx & 2;
    bool x = (word_t)rwx & 1;

    pg->set_entry(this, pgsize, paddr, r, w, x, kernel, attrib);
    arm_cache::cache_drain_write_buffer();

    return true;
}

extern "C" void NORETURN SECTION(".init") init_memory(word_t *physbase)
{
    extern char _kernel_space_pagetable[];

    word_t i;
    addr_t start, end;

    pgent_t * kspace_phys = (pgent_t *)virt_to_phys_init( _kernel_space_pagetable, physbase);

    /* Zero out the level 1 translation table */
    for (i = 0; i < ARM_HWL1_SIZE/sizeof(word_t); ++i)
        ((word_t*)kspace_phys)[i] = 0;

    map_phys_memory(kspace_phys, physbase);

    /* Enable kernel domain (0) */
    write_cp15_register(C15_domain, C15_CRm_default, C15_OP2_default, 0x0001);
    /* Set the translation table base to use the kspace_phys */
    write_cp15_register(C15_ttbase, C15_CRm_default, 0, (word_t) kspace_phys);

    /* Map the current code area 1:1 */
    start = addr_align(virt_to_phys_init(start_init, physbase), PAGE_SIZE_1M);
    end = virt_to_phys_init(end_init, physbase);
    for (i = (word_t)start; i < (word_t)end; i += PAGE_SIZE_1M)
    {
        add_mapping_init( kspace_phys, (addr_t)i, (addr_t)i, uncached );
    }

    /* Enable virtual memory, caching etc */
    write_cp15_register(C15_control, C15_CRm_default,C15_OP2_default, C15_CONTROL_KERNEL);

    /* Initialize global pointers (requres 1:1 mappings for physbase) */
    init_arm_globals(physbase);

    run_init_script(INIT_PHASE_FIRST_HEAP);

    static char _kernel_top_level[ARM_L0_SIZE];

    pgent_t * current = (pgent_t *)_kernel_top_level;
    /* Link kernel space top level to CPD */
    for (i = 0; i < ARM_L0_ENTRIES; i++, current++)
        current->tree = ((pgent_t *) virt_to_page_table(_kernel_space_pagetable)) + i * ARM_L1_ENTRIES;

    /* Initialize the kernel space data structure and link it to the pagetable */
    space_t::set_kernel_page_directory((pgent_t *)_kernel_top_level);

    space_t * kspace = get_kernel_space();

    kmem_resource_t *kresource = get_current_kmem_resource();

    bool r;
    /* Map the UTCB reference page */
    r = kspace->add_mapping((addr_t)USER_UTCB_PAGE, virt_to_phys((addr_t) arm_utcb_page),UTCB_AREA_PGSIZE, space_t::read_execute, false, kresource);

    init_arm_interrupts();
    init_idle_tcb();
    get_arm_globals()->current_tcb = get_idle_tcb();
    pre_tcb_init = 0;
    run_init_script(INIT_PHASE_OTHERS);
    get_current_scheduler()->schedule(get_current_tcb(), idle_thread, scheduler_t::sched_default);   
}



