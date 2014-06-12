/*
 * Description:   Resource bit definitions for arm
 */

#ifndef __GLUE__V4_ARM__RESOURCES_H__
#define __GLUE__V4_ARM__RESOURCES_H__

#define ARM_FPA         1
#define ARM_VFP         2
#define ARM_IWMMX       4

#define HAVE_RESOURCE_TYPE_E
enum resource_type_e {
    KIPC = 0,           /* Used for decisions in fastpath */
    EXCEPTIONFP = 1,    /* Used for decisions in fastpath */
};


class thread_resources_t : public generic_thread_resources_t
{
public:
    void dump(tcb_t * tcb);
    void save(tcb_t * tcb);
    void load(tcb_t * tcb);
    void purge(tcb_t * tcb);
    void init(tcb_t * tcb);
    void free(tcb_t * tcb, kmem_resource_t *kresource);

    void set_kernel_ipc(tcb_t * tcb);
    void clear_kernel_ipc(tcb_t * tcb);
    void clear_except_fp(tcb_t * tcb);

    bool control(tcb_t * tcb, tc_resources_t resources,
                 kmem_resource_t *kresource);
};


#endif /* !__GLUE__V4_ARM__RESOURCES_H__ */
