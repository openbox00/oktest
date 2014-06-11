/*
 * Description:   Conversion functions for hardware space addresses
 */
#ifndef __ARCH_ARM__HWSPACE_H__
#define __ARCH_ARM__HWSPACE_H__

#include <arch/globals.h>

template<typename T> INLINE T virt_to_phys(T x)
{
        return (T) ((u32_t) x - VIRT_ADDR_RAM + get_arm_globals()->phys_addr_ram);
}

template<typename T> INLINE T phys_to_virt(T x)
{
        return (T) ((u32_t) x + VIRT_ADDR_RAM - get_arm_globals()->phys_addr_ram);
}

template<typename T> INLINE T virt_to_ram(T x)
{
    return (T) ((u32_t) x - VIRT_ADDR_RAM + get_arm_globals()->phys_addr_ram);
}

template<typename T> INLINE T ram_to_virt(T x)
{
    return (T) ((u32_t) x + VIRT_ADDR_RAM - get_arm_globals()->phys_addr_ram);
}

#endif /*__ARCH_ARM__HWSPACE_H__*/
