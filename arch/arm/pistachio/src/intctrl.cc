/*
 * Description:   Implementation of interrupt control functionality
 */

#include <l4.h>
#include <tcb.h>
#include <interrupt.h>
#include <arch/intctrl.h>
#include <schedule.h>
#include <cpu/syscon.h>

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

#ifdef CONFIG_DISABLE_ALIGNMENT_EXCEPTIONS
    control &= ~((word_t)C15_CONTROL_A);
#else
    control |= C15_CONTROL_A;
#endif
    write_cp15_register(C15_control, C15_CRm_default, C15_OP2_default, control);

    /* Read control register again - hardware may not support A-bit */
    read_cp15_register(C15_control, C15_CRm_default, C15_OP2_default, control);

    /* Initialize cached values in VECTOR page */
    extern tcb_t** vector_tcb_handle_array, **thread_handle_array;
    extern word_t vector_num_tcb_handles, num_tcbs;

    /* Cache these values */
    vector_num_tcb_handles = num_tcbs;
    vector_tcb_handle_array = thread_handle_array;
}

