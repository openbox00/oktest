#include <tcb.h>

tcb_t*
allocate_tcb(capid_t tid, kmem_resource_t *kresource)
{
    tcb_t* ret = (tcb_t *)kresource->alloc(kmem_group_tcb, true);
    return ret;    
}
/**
 * Initialize a new thread
 */
void tcb_t::init(void)
{
    init_stack();
}


void tcb_t::create_startup_stack (void (*func)())
{
    init_stack();
    notify(func);
    arm_irq_context_t *context = &(arch.context);
    context->cpsr = CPSR_USER_MODE;
}

bool tcb_t::activate(void (*startup_func)(), kmem_resource_t *kresource)
{
    this->utcb = get_space()->allocate_utcb(this, kresource);
    create_startup_stack(startup_func);
    return true;
}

