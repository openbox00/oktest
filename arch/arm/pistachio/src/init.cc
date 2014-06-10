/*
 * Description:   ARM specific initialization code
 */

#include <l4.h>
#include <debug.h>
#include <schedule.h>
#include <space.h>
#include <arch/memory.h>
#include <interrupt.h>
#include <config.h>
#include <arch/hwspace.h>
#include <cpu/syscon.h>
#include <arch/init.h>
#include <kernel/generic/lib.h>
#include <soc/soc.h>
#include <soc/arch/soc.h>
#include <soc/interface.h>

extern "C" void startup_system_mmu();

#if !defined(CONFIG_ARM_THREAD_REGISTERS)
/* UTCB reference page. This needs to be revisited */
ALIGNED(UTCB_AREA_PAGESIZE) char arm_utcb_page[UTCB_AREA_PAGESIZE] UNIT("utcb");
#endif

#if defined(CONFIG_TRUSTZONE)

/* System Mode: Are we running in secure or non-seceure mode */
bool arm_secure_mode;

#endif


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


/**
 * Get page table index of virtual address for a 1m size
 *
 * @param vaddr         virtual address
 *
 * @return page table index for a table of the given page size
 */
INLINE word_t page_table_1m (addr_t vaddr)
{
    return ((word_t) vaddr >> 20) & (ARM_HWL1_SIZE - 1);
}

/*
 * Add 1MB mappings during init.
 * This function works with the mmu disabled
 */
extern "C" void SECTION(".init") add_mapping_init(pgent_t *pdir, addr_t vaddr,
                addr_t paddr, memattrib_e attrib)
{
    pgent_t *pg = pdir + (page_table_1m(vaddr));

    pg->set_entry_1m(NULL, paddr, true, true, true, attrib);
}

extern "C" void SECTION(".init") add_rom_mapping_init(pgent_t *pdir, addr_t vaddr,
                addr_t paddr, memattrib_e attrib)
{
    pgent_t *pg = pdir + (page_table_1m(vaddr));

    pg->set_entry_1m(NULL, paddr, true, false, true, attrib);
}

SECTION(".init")
void map_phys_memory(pgent_t *kspace_phys, word_t *physbase)
{
   /* The first block of code only maps RAM that exists. This is useful
    * for debugging purposes. However the second block of code gives
    * faster initialization and less code footprint, so is used by default.
    */
    for (word_t j = (word_t)VIRT_ADDR_BASE; j < (word_t)VIRT_ADDR_BASE + KERNEL_AREA_SIZE; j += PAGE_SIZE_1M)
        {
            word_t phys = virt_to_phys_init(j, physbase);
            add_mapping_init( kspace_phys, (addr_t)j, (addr_t)phys, writeback );
            add_mapping_init( kspace_phys, (addr_t)phys_to_page_table_init(phys, physbase), (addr_t)phys, writethrough );
        }
}

/*
 * Setup the kernel page table and map the kernel and data areas.
 *
 * This function is entered with the MMU disabled and the code
 * running relocated in physical addresses. We setup the page
 * table and bootstrap ourselves into virtual mode/addresses.
 */
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
    r = kspace->add_mapping((addr_t)USER_UTCB_PAGE, virt_to_phys((addr_t) arm_utcb_page),
                            UTCB_AREA_PGSIZE, space_t::read_execute, false, kresource);

    jump_to((word_t)startup_system_mmu);
}

NORETURN void SECTION(".init") generic_init();

extern "C" void SECTION(".init") startup_system_mmu()
{
    /* Configure IRQ hardware */
    init_arm_interrupts();
    init_idle_tcb();
    get_arm_globals()->current_tcb = get_idle_tcb();
    /* Architecture independent initialisation. Should not return. */
    generic_init();

    NOTREACHED();
}



