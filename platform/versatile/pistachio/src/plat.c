/*
 * Description:   ARM versatile platform specfic functions
 */

#include <soc/soc.h>
#include <soc/interface.h>
#include <soc.h>
#include <reg.h>
#include <interrupt.h>

addr_t versatile_io_vbase;
addr_t versatile_uart0_vbase;

/* SDRAM region */
#define SDRAM_START             0x04000000
#define SDRAM_END               0x08000000

#define VERSATILE_UART0_PEND    (VERSATILE_UART0_PBASE + 0x1000)

/*---------------------------------------------------------------------------
  I/O Virtual to Physical mapping
---------------------------------------------------------------------------*/
#define MAP_ENTRY(base,n) { base + n * ARM_SECTION_SIZE, base##_PHYS + n * ARM_SECTION_SIZE}
#define SOC_CACHE_DEVICE 0x0

#define SIZE_4K 4096

word_t soc_api_version = SOC_API_VERSION;

/*
 * Initialize the platform specific mappings needed
 * to start the kernel.
 * Add other hardware initialization here as well
 */
void SECTION(".init")
soc_init(void)
{
    volatile vic_t *sic = NULL;
    /* Map peripherals and control registers */

    versatile_io_vbase = kernel_add_mapping(SIZE_4K,
                                            (addr_t)VERSATILE_IO_PBASE,
                                            SOC_CACHE_DEVICE);
    versatile_sctl_vbase = kernel_add_mapping(SIZE_4K,
                                              (addr_t)VERSATILE_SCTL_PBASE,
                                              SOC_CACHE_DEVICE);
    versatile_timer0_vbase = kernel_add_mapping(SIZE_4K,
                                                (addr_t)VERSATILE_TIMER0_PBASE,
                                                SOC_CACHE_DEVICE);
    versatile_vic_vbase = kernel_add_mapping(SIZE_4K,
                                             (addr_t)VERSATILE_VIC_PBASE,
                                             SOC_CACHE_DEVICE);
    versatile_sic_vbase = kernel_add_mapping(SIZE_4K,
                                             (addr_t)VERSATILE_SIC_PBASE,
                                             SOC_CACHE_DEVICE);
    versatile_uart0_vbase = kernel_add_mapping(SIZE_4K,
                                               (addr_t)VERSATILE_UART0_PBASE,
                                               SOC_CACHE_DEVICE);


    /* Clear all SIC interrupt forwards */
    sic = (vic_t *)versatile_sic_vbase;
    sic->protect = 0UL;
    init_clocks();
}

word_t soc_do_platform_control(tcb_h current,
                               plat_control_t control,
                               word_t param1,
                               word_t param2,
                               word_t param3,
                               continuation_t cont)
{
    utcb_t *current_utcb = kernel_get_utcb(current);
    current_utcb->error_code = ENOT_IMPLEMENTED;
    return 0;
}

