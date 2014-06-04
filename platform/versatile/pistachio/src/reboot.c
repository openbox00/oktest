/*
 * Description: ARM versatile reboot
 */

#include <soc/soc.h>
#include <soc/arch/soc.h>
#include <reg.h>
#include <soc.h>

void soc_panic(void)
{
    /*
     * To reset, we hit the on-board reset register
     * in the system FPGA
     */
    word_t val = *((volatile word_t *) VERSATILE_SYS_RESETCTL_VADDR);
    *((volatile word_t *) VERSATILE_SYS_RESETCTL_VADDR) = val | VERSATILE_SYS_CTRL_RESET_CONFIGCLR;

    for (;;);
}
