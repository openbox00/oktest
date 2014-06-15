#ifndef __ARM_ARCH_KERNEL_CONFIG_H__
#define __ARM_ARCH_KERNEL_CONFIG_H__

#define         VIRT_ADDR_BASE          0xF0000000
#define         VIRT_ADDR_PGTABLE       0xF4000000

#define         VIRT_ADDR_ROM           VIRT_ADDR_BASE
#define         VIRT_ADDR_RAM           VIRT_ADDR_BASE


#define STACK_BITS      10
#define STACK_SIZE      (1 << STACK_BITS)
/* to get the top of the stack logical or any valid sp with this mask */
#define STACK_TOP       (STACK_SIZE - 8)
/* on arm the stack top mask must be split in two for use in assembler */
#define STACK_TEST              (STACK_TOP)

#endif
