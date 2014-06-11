/*
 * Description:  ARCH Kernel Resource Manager
 */
#include <kmem_resource.h>
#include <arch/pgent.h>

void kmem_resource_t::arch_init_small_alloc_pools()
{
    small_alloc_pools[kmem_group_l0_allocator].init(&kmem_groups[kmem_group_l0_allocator], ARM_L0_SIZE);
}
