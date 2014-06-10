/*
 * Description:  Versatile Platform Interrupt Handling
 */

#include <soc/interface.h>
#include <soc/soc.h>
#include <kernel/arch/special.h>
#include <interrupt.h>
#include <reg.h>
#include <soc.h>
#include <kernel/arch/continuation.h>

addr_t versatile_vic_vbase;
addr_t versatile_sic_vbase;

bool soc_timer_disabled = false;

/* Mask a SIC irq. */
static void
mask_sic_irq(word_t irq)
{
    volatile sic_t *sic = (sic_t *)versatile_sic_vbase;
    sic->irq_enable_clear = (1UL << irq);
}

/* Unmask a SIC irq. */
static void
unmask_sic_irq(word_t irq)
{
    volatile sic_t *sic = (sic_t *)versatile_sic_vbase;
    sic->irq_enable = (1UL << irq);
}

/* Mask an IRQ. */
void
soc_mask_irq(word_t irq)
{
    volatile vic_t  *vic = (vic_t *)versatile_vic_vbase;
    if (irq <= VERSATILE_SIC_IRQ) {
        vic->irq_enable_clear = (1UL << irq);
    } else {
        mask_sic_irq(irq - VERSATILE_SIC_OFFSET);
    }
}

/* Unmask an IRQ. */
void
soc_unmask_irq(word_t irq)
{
    volatile vic_t  *vic = (vic_t *)versatile_vic_vbase;
    if (irq <= VERSATILE_SIC_IRQ) {
        vic->irq_enable |= (1UL << irq);
    } else {
        unmask_sic_irq(irq - VERSATILE_SIC_OFFSET);
    }
}

/* Disable the system timer. */
void
soc_disable_timer(void)
{
    soc_timer_disabled = 1;
    soc_mask_irq(VERSATILE_TIMER0_IRQ);
}

/* Enable the system timer. */
void
soc_enable_timer(void)
{
    soc_timer_disabled = 0;
    soc_unmask_irq(VERSATILE_TIMER0_IRQ);
}

