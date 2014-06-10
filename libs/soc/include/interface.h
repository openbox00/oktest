/**
 * @file
 * @brief Functions provided by the general kernel that platform code may
 * call.
 *
 * Calling other functions from the platform code is unsupported.
 */

#ifndef __PLATFORM_SUPPORT_H__
#define __PLATFORM_SUPPORT_H__

/*
 * NOTE: platform code should not include any kernel headers directly.  All
 * the ones which are needed are included here.
 */
#include <soc/soc_types.h>
#include <kernel/types.h>


#ifndef ASSEMBLY
#include <kernel/bitmap.h>
#include <kernel/generic/lib.h>
#include <kernel/errors.h>

extern word_t kernel_api_version;
typedef struct
{
    tcb_h  object;   /**< The stored tcb */
    word_t reserved; /**< NOT for SOC usage */
} soc_ref_t;
#define NULL_TCB 0
INLINE tcb_h kernel_ref_get_tcb(soc_ref_t ref)
{
    return ref.object;
}
void kernel_ref_init(soc_ref_t *ref);
void kernel_ref_set_referenced(tcb_h      obj,
                               soc_ref_t *ref);
void kernel_ref_remove_referenced(soc_ref_t *ref);
tcb_h kernel_lookup_tcb_locked(word_t cap);
//void kernel_unlock_tcb(tcb_h);
utcb_t *kernel_get_utcb(tcb_h obj);
bool kernel_deliver_notify(tcb_h          handler,
                           word_t         notifybits,
                           continuation_t cont);
void kernel_schedule(continuation_t cont) NORETURN;
void kernel_scheduler_handle_timer_interrupt(int            wakeup,
                                             word_t         interval,
                                             continuation_t cont) NORETURN;
void * kernel_mem_alloc(word_t size, int zero);
void kernel_mem_free(void * address, word_t size);
addr_t kernel_add_mapping(word_t size,
                          addr_t phys,
                          word_t cache_attr);

bool kernel_add_mapping_va(addr_t virt,
                           word_t size,
                           addr_t phys,
                           word_t cache_attr);
void kernel_fatal_error(const char *msg) NORETURN;

void kernel_arch_init(void *param) NORETURN;

#endif

#include <kernel/arch/asm.h>
#include <kernel/arch/continuation.h>


#endif /*__PLATFORM_SUPPORT_H__*/
