/*
 * Description:   ARMv5 thread resources
 */

#include <l4.h>
#include <tcb.h>
#include <arch/resource_functions.h>
#include <space.h>
#include <debug.h>
#include <kernel/bitmap.h>

void thread_resources_t::free(tcb_t * tcb, kmem_resource_t *kresource)
{
#if 0
    if (vfp_state)
    {
        arm_vfp_free(tcb, kresource);
    }
#endif

    /* We have to free the thread's UTCB */
    space_t *space = tcb->get_space();

    space->free_utcb(tcb->get_utcb());
}

