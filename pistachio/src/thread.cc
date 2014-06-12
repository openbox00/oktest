/*
 * Description:   thread manipulation
 */

#include <l4.h>
#include <config.h>
#include <tcb.h>
#include <ipc.h>
#include <capid.h>
#include <schedule.h>
#include <space.h>
#include <linear_ptab.h>
#include <kernel/generic/lib.h>
#include <ref_inline.h>

extern "C" CONTINUATION_FUNCTION(initial_to_user);
void handle_ipc_error (void);

INLINE addr_t addr_align (addr_t addr, word_t align)
{
    return addr_mask (addr, ~(align - 1));
}


CONTINUATION_FUNCTION(thread_startup)
{
    tcb_t * current = get_current_tcb();

    current->set_user_ip((addr_t)current->get_mr(1));
    current->set_user_sp((addr_t)current->get_mr(2));
    current->set_saved_state(thread_state_t::aborted);
    current->sys_data.set_action(tcb_syscall_data_t::action_ipc);
    ACTIVATE_CONTINUATION(initial_to_user);
}

#define END_TCB_FREE_LIST 0x0FFFFFFFUL

tcb_t*
allocate_tcb(capid_t tid, kmem_resource_t *kresource)
{
    /* Get the head of the free list */
    extern word_t free_tcb_idx;
    extern tcb_t** thread_handle_array;
    word_t me = free_tcb_idx;
    /* if there is a free tcb */
    if (me != END_TCB_FREE_LIST) {
        /* advance the free pointer */
        tcb_t* ret = (tcb_t *)kresource->alloc(kmem_group_tcb, true);
        if (ret != NULL)
        {
            free_tcb_idx = (word_t)thread_handle_array[me];
            thread_handle_array[me] = ret;
            /* set up the allocated tcb */
            ret->tcb_idx = me;
        }
        return ret;
    } else {
        return NULL;
    }
}
#if 0
void
free_tcb(tcb_t *tcb)
{
    /* Add the tcb to the head of the free list */
    extern word_t free_tcb_idx;
    extern tcb_t** thread_handle_array;

    thread_handle_array[tcb->tcb_idx] = (tcb_t *)free_tcb_idx;
    free_tcb_idx = tcb->tcb_idx;
    /* Free tcb */
    get_current_kmem_resource()->free(kmem_group_tcb, tcb);
}
#endif
/**
 * Initialize a new thread
 */
void tcb_t::init(void)
{
    /* clear utcb and space */
    utcb = NULL;
    space = NULL;

    /* make sure, nobody messes around with the thread */
    initialise_state(thread_state_t::aborted);
    set_saved_state(get_state());
    partner = NULL;
    set_saved_partner(NULL);

    /* Initialize queues */
    ready_list.next = NULL;
    blocked_list.next = NULL;

    /* flags */
    runtime_flags = 0;
    post_syscall_callback = NULL;

    /* IPC Control initialization */
    space_id = spaceid_t::kernelspace();

    /* initialize scheduling */
    get_current_scheduler()->init_tcb(this);

    init_stack();

    this->pager.init();
    this->exception_handler.init();
}


bool SECTION(SEC_INIT)
tcb_t::create_kernel_thread(utcb_t * new_utcb)
{
    this->init();
    this->utcb = new_utcb;
    this->utcb_location = (word_t)new_utcb;
    this->page_directory = get_kernel_space()->pgent(0);

    get_current_scheduler()->set_priority(this, 0);

    get_kernel_space()->add_tcb(this);
    return true;
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
    if (!this->utcb)
        return false;
    this->space_id = get_space()->get_space_id();

    /* initialize the startup stack */
    create_startup_stack(startup_func);
    return true;
}
/* Assumes thread_state_lock is held by caller */
void tcb_t::save_state (word_t mrs)
{
    for (word_t i = 0; i < mrs; i++) {
        TCB_SYSDATA_IPC(this)->saved_mr[i] = get_mr(i);
    }

    TCB_SYSDATA_IPC(this)->saved_notify_mask = get_notify_mask();
    TCB_SYSDATA_IPC(this)->saved_error = get_error_code();
    saved_partner = partner;
    saved_sent_from = sent_from;

    set_saved_state (get_state ());
}

void
tcb_t::restore_state(word_t mrs)
{
    thread_state_t saved_state = get_saved_state();
    scheduler_t * scheduler = get_current_scheduler();

    for (word_t i = 0; i < mrs; i++) {
        set_mr(i, TCB_SYSDATA_IPC(this)->saved_mr[i]);
    }
    set_notify_mask(TCB_SYSDATA_IPC(this)->saved_notify_mask);

    sent_from = saved_sent_from;
    partner = saved_partner;

    if (get_state().is_runnable()) {
        if (saved_state.is_runnable()) {
            scheduler->update_active_state(this, saved_state);
        } else {
            scheduler->deactivate(this, saved_state);
        }
    } else {
        if (saved_state.is_runnable()) {
            scheduler->activate(this, saved_state);
        } else {
            scheduler->update_inactive_state(this, saved_state);
        }
    }
    set_error_code(TCB_SYSDATA_IPC(this)->saved_error);

    set_saved_partner(NULL);
    set_saved_state(thread_state_t::aborted);
}
extern "C" void
start_post_syscall_callback(void)
{
    continuation_t cont = ASM_CONTINUATION;
    tcb_t * current = get_current_tcb();
    current->get_post_syscall_callback()(cont);

    ACTIVATE_CONTINUATION(cont);
}
