/*
 * Description:   Periodic timer handling
 */

#include <soc/soc.h>
#include <soc/interface.h>
#include <interrupt.h>
#include <reg.h>
#include <soc.h>

addr_t versatile_timer0_vbase;
addr_t versatile_sctl_vbase;

void init_clocks(void)
{
    volatile Timer_t    *timer0 = (Timer_t *)VERSATILE_TIMER0_VBASE;
    volatile Timer_t    *timer1 = (Timer_t *)VERSATILE_TIMER1_VBASE;
    volatile SysCtl_t   *sysctl = (SysCtl_t *)VERSATILE_SCTL_VBASE;
    sysctl->ctrl = VERSATILE_CLOCK_MODE_NORMAL |
                    (1UL << VERSATILE_BIT_TIMER_EN0_SEL) |
                    (1UL << VERSATILE_BIT_TIMER_EN1_SEL) |
                    (1UL << VERSATILE_BIT_TIMER_EN2_SEL) |
                    (1UL << VERSATILE_BIT_TIMER_EN3_SEL);
    timer0->load = VERSATILE_TIMER_RELOAD;
    soc_unmask_irq(VERSATILE_TIMER0_IRQ);
    timer0->ctrl = VERSATILE_TIMER_CTRL | VERSATILE_TIMER_MODE | VERSATILE_TIMER_IE;
    timer1->ctrl = VERSATILE_TIMER_CTRL | VERSATILE_TIMER_32BIT;
    timer1->clear = 0;

}
