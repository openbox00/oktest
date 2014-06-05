#include <l4.h>
#include <arch/syscalls.h>
#include <soc/interface.h>
#include <tcb.h>
#include <caps.h>
#include <kmem_resource.h>
#include <ref_inline.h>

class tcb_t;

word_t kernel_api_version = SOC_API_VERSION;

extern "C" void
kernel_ref_init(soc_ref_t *ref)
{
    ref_t *kernel_ref = (ref_t *)ref;
    kernel_ref->init();
}

extern "C"
tcb_h kernel_lookup_tcb_locked(word_t cap)
{
    capid_t tid;
    tid.set_raw(cap);
    return (tcb_h)(get_current_clist()->lookup_thread_cap_locked(tid));
}

extern "C"
void kernel_unlock_tcb(tcb_h obj)
{
    tcb_t *tcb = (tcb_t *)obj;
    tcb->unlock_read();
}

extern "C" void
kernel_ref_set_referenced(tcb_h obj, soc_ref_t *ref)
{
    ref_t *kernel_ref = (ref_t *)ref;
    tcb_t *tcb = (tcb_t *)obj;
    kernel_ref->set_referenced(tcb);
}

extern "C" void
kernel_ref_remove_referenced(soc_ref_t *ref)
{
    ref_t *kernel_ref = (ref_t *)ref;
    tcb_t *tcb = kernel_ref->get_tcb();
    kernel_ref->remove_referenced(tcb);
}

extern "C" utcb_t *
kernel_get_utcb(tcb_h obj)
{
    return ((tcb_t *)obj)->get_utcb();
}

extern "C"
void * kernel_mem_alloc(word_t size, int zeroed)
{
    return get_current_kmem_resource()->alloc(kmem_group_irq, size, zeroed);
}

extern "C"
void kernel_mem_free(void * address, word_t size)
{
    get_current_kmem_resource()->free(kmem_group_irq, address, size);
}
