/*
 * Description: Versatile platform interrupt handling
 */
#ifndef __PLATFORM__VERSATILE__INTERRUPT_H__
#define __PLATFORM__VERSATILE__INTERRUPT_H__

#include <soc/soc_types.h>

/* Use simplesoc. */
#define USE_SIMPLESOC     1

#define IRQS  64

#define VERSATILE_SIC_IRQ       31
#define VERSATILE_SIC_OFFSET    32
#define SIC_FORWARD_START       21
#define SIC_FORWARD_END         26

/* versatile interrupt handling utilities */
void soc_init_interrupts(void);
void soc_mask_irq(word_t irq);
void soc_unmask_irq(word_t irq);

#endif

