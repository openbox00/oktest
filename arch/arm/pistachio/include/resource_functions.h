#ifndef __GLUE__V4_ARM__RESOURCE_FUNCTIONS_H__
#define __GLUE__V4_ARM__RESOURCE_FUNCTIONS_H__

#include <arch/resources.h>
#include <tcb.h>

INLINE void thread_resources_t::set_kernel_ipc (tcb_t * tcb)
{
    tcb->resource_bits += KIPC;
}

INLINE void thread_resources_t::clear_kernel_ipc (tcb_t * tcb)
{
    tcb->resource_bits -= KIPC;
}

INLINE void thread_resources_t::clear_except_fp(tcb_t * tcb)
{
    tcb->resource_bits -= EXCEPTIONFP;
}


INLINE void thread_resources_t::save(tcb_t * tcb)
{
}

INLINE void thread_resources_t::load(tcb_t * tcb)
{
}

INLINE bool thread_resources_t::control(tcb_t * tcb, tc_resources_t resources,
                                        kmem_resource_t *kresource)
{
    return true;
}

INLINE void thread_resources_t::dump(tcb_t * tcb)
{
}

#endif /* __GLUE__V4_ARM__RESOURCE_FUNCTIONS_H__ */
