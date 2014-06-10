/*
 * Description:   thread manipulation
 */

#include <l4.h>
#include <config.h>
#include <tcb.h>
#include <ipc.h>
#include <arch/ipc.h>
#include <capid.h>
#include <schedule.h>
#include <space.h>
#include <syscalls.h>
#include <linear_ptab.h>
#include <kernel/generic/lib.h>
#include <ref_inline.h>

extern "C" CONTINUATION_FUNCTION(initial_to_user);
void handle_ipc_error (void);

CONTINUATION_FUNCTION(thread_startup)
{
    tcb_t * current = get_current_tcb();

    current->set_user_ip((addr_t)current->get_mr(1));
    current->set_user_sp((addr_t)current->get_mr(2));
    current->set_saved_state(thread_state_t::aborted);
    current->sys_data.set_action(tcb_syscall_data_t::action_ipc);
    ACTIVATE_CONTINUATION(initial_to_user);
}


static void
fake_wait_for_startup(tcb_t * tcb)
{
    tcb->initialise_state(thread_state_t::waiting_forever);
    tcb->set_partner(tcb->get_pager());
    tcb->set_acceptor(acceptor_t::untyped_words());

    tcb->set_saved_partner(NULL);

    tcb->set_saved_state (thread_state_t::running);
}

#define END_TCB_FREE_LIST 0x0FFFFFFFUL

/*
 * Initialize TCB memory allocator, setting object
 * size and max number of objects
 */
void SECTION(SEC_INIT)
init_tcb_allocator(void)
{
    extern word_t num_tcbs;
    extern tcb_t** thread_handle_array;
    word_t i;

    /* Initialize free tcb list */
    for (i = 0; i < num_tcbs; i++)
    {
        thread_handle_array[i] = (tcb_t *)(i+1);
    }
    thread_handle_array[num_tcbs - 1] = (tcb_t *)END_TCB_FREE_LIST;
}

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


    /* Lock initialization */
    this->thread_lock.init();

    /* IPC Control initialization */
    space_id = spaceid_t::kernelspace();

    /* initialize scheduling */
    get_current_scheduler()->init_tcb(this);

    /* enqueue into present list, do not enqueue
     * the idle thread since they are CPU local and break
     * the pointer list */
    enqueue_present();

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

bool tcb_t::activate(void (*startup_func)(), kmem_resource_t *kresource)
{
    this->utcb = get_space()->allocate_utcb(this, kresource);
    if (!this->utcb)
        return false;

#ifdef CONFIG_PLAT_SURF
    /* Init debug profiling */
    prof_set_id_init(this->utcb);
#endif

    this->space_id = get_space()->get_space_id();

    /* update mutex thread handle in UTCB */
    set_mutex_thread_handle(threadhandle(this->tcb_idx));

    /* initialize the startup stack */
    create_startup_stack(startup_func);
    return true;
}

/**
 * Check if supplied address is a valid UTCB location.  It is assumed
 * that the space of the thread is properly initialized.
 *
 * @param utcb_address          location of UTCB
 *
 * @return true if UCTB location is valid, false otherwise
 */
bool generic_space_t::check_utcb_location (word_t utcb_address)
{
#ifdef NO_UTCB_RELOCATE
    /* UTCB is kernel allocated, utcb_address must be -1UL */
    return utcb_address == ~0UL;
#else
    return (((word_t)addr_align((addr_t)utcb_address, 
                                CONFIG_UTCB_SIZE) == utcb_address) &&
            get_utcb_area().is_range_in_fpage((addr_t) utcb_address,
                                              (addr_t) (utcb_address +
                                                        sizeof (utcb_t))));
#endif
}

/**
 * Cancel all pending IPCs related to this thread, and change the thread
 * into an aborted state.
 *
 * This will unwind the IPC of any thread currently sending to us, and if we
 * are currently blocked on a send or receive, cancel that as well.
 *
 * Function must be called with current thread locked.
 */
void
tcb_t::cancel_ipcs(void)
{
    /* Unwind any IPC that we happen to be in the middle of. */
    if (this->is_partner_valid())
    {
        tcb_t *partner = get_partner();
        if (partner == this) {
            this->unwind(partner);
        } else if (((word_t)partner < SPECIAL_RAW_LIMIT) && this->get_space()) {
            this->unwind(partner);
        }
        /* do nothing for partner == anythread / kernel / uninitialized threads */
    }

    /* Cancel any IPCs of other threads that are sending to us. */
    while (this->end_point.get_send_head()) {
        tcb_t * tcb = this->end_point.get_send_head();

        /* Unwind the sender's IPC, and notify them of the error. */
        get_current_scheduler()->pause(tcb);
        tcb->unwind(this);
        tcb->notify(handle_ipc_error);
        get_current_scheduler()->activate(tcb, thread_state_t::running);
    }

    /* Cancel any IPCs of other threads that are receiving from us. */
    while (this->end_point.get_recv_head()) {
        tcb_t * tcb = this->end_point.get_recv_head();

        /* Unwind the receiver's IPC, and notify them of the error. */
        get_current_scheduler()->pause(tcb);
        tcb->unwind(this);
        tcb->notify(handle_ipc_error);
        get_current_scheduler()->activate(tcb, thread_state_t::running);
    }
}

/**
 * Release any mutexes that we currently hold.
 *
 * @return  A boolean value indicating if a schedule() call is required.
 */
bool
tcb_t::release_mutexes(void)
{
    bool need_reschedule = false;
    scheduler_t * scheduler = get_current_scheduler();

    /* If we are blocked on a mutex, unwind that. */
    if (get_state().is_waiting_mutex()) {
        this->unwind(NULL);
        get_current_scheduler()->activate(this, thread_state_t::running);
        need_reschedule = true;
    }

    /* Clean up all currently held mutexes. */
    mutex_t * mutex;

    while ((mutex = this->mutexes_head) != NULL) {
        tcb_t * new_holder;

        /* If the mutex has a new holder, make them runnable. */
        if (new_holder != NULL) {
            scheduler->activate(new_holder, thread_state_t::running);
            need_reschedule = true;
        }
    }
    return need_reschedule;
}

/**
 * Try to aquire a read-lock on a thread
 *
 * @param tcb tcb of thread to try lock
 *
 * @returns NULL if thread's cap deleted, tcb if lock acquired
 */
tcb_t *
acquire_read_lock_tcb(tcb_t *tcb, tcb_t *tcb_locked)
{
    cap_t lookup;
    tcb_t *cap_tcb;

try_valid_cap_lookup:
    okl4_atomic_barrier_smp();
    cap_t *master_cap = tcb->master_cap;

    if (EXPECT_TRUE(master_cap != NULL)) {
        lookup = *master_cap;

        if (EXPECT_FALSE(!lookup.is_thread_cap())) {
            return NULL;
        }

        cap_tcb = lookup.get_tcb();

        /* if cap still points to this tcb */
        if (EXPECT_TRUE(cap_tcb == tcb)) {
            if (tcb == tcb_locked) {
                tcb->lock_read_already_held();
            } else if (EXPECT_FALSE(!tcb->try_lock_read())) {
                okl4_atomic_barrier_smp();
                goto try_valid_cap_lookup;
            }
            return tcb;
        }
    }
    return NULL;
}

/**
 * Lookup a TCB by its thread-handle
 *
 * @param threadhandle Thread handle of thread to lookup
 *
 * @returns NULL if non-existing, else tcb (locked)
 */
tcb_t*
lookup_tcb_by_handle_locked(word_t threadhandle)
{
    extern tcb_t ** thread_handle_array;
    extern word_t num_tcbs;
    tcb_t *tcb;

    word_t idx = (threadhandle << 1) >> 1;

    if (idx < (num_tcbs))
    {
        if (((word_t)thread_handle_array[threadhandle] > VIRT_ADDR_BASE))
        {
            tcb = thread_handle_array[threadhandle];
            return acquire_read_lock_tcb(tcb);
        }
        else
            return NULL;
    }
    else
    {
        return NULL;
    }
}

/**
 * Delete a thread
 */
void
tcb_t::delete_tcb(kmem_resource_t *kresource)
{
    scheduler_t *scheduler = get_current_scheduler();

    scheduler->pause(this);

    this->cancel_ipcs();

    /* Unwind ourselves thread into an aborted state. */
    this->unwind(NULL);

    /* Release any syncpoints owned by the thread. */
    (void)this->release_mutexes();

    if (get_active_schedule() == this) {
        set_active_schedule(get_current_tcb());
    }
    scheduler->delete_tcb(this);

    // free any used resources
    resources.free(this, kresource);

    set_pager(NULL);
    set_exception_handler(NULL);

    this->set_space(NULL);
    dequeue_present();

#if defined(CONFIG_THREAD_NAMES)
    debug_name[0] = '\0';
#endif
    sys_data.set_action(tcb_syscall_data_t::action_none);
    free_tcb(this);

}

INLINE void
calculate_errorcodes( tcb_t * snd, tcb_t * rcv,
                     word_t * err_s, word_t * err_r)
{
    *err_s = IPC_SND_ERROR(ERR_IPC_ABORTED);
    *err_r = IPC_RCV_ERROR(ERR_IPC_ABORTED);
}

void
tcb_t::unwind(tcb_t *partner)
{
    thread_state_t orig_cstate = get_state();
    thread_state_t orig_sstate = get_saved_state();
    tcb_t * orig_cpartner UNUSED = is_partner_valid() ? get_partner() : NULL;
    tcb_t * orig_spartner UNUSED = get_saved_partner();

    thread_state_t cstate = get_state();

    if (cstate.is_running()) {
        /* Abort the thread. */
        get_current_scheduler()->deactivate(this, thread_state_t::aborted);
        return;
    }

    if (cstate.is_aborted()) {
        /* Thread is already aborted. */
        return;
    }

    if (cstate.is_halted()) {
        /* Thread is halted. Transition it to the aborted state. */
        get_current_scheduler()->update_inactive_state(this,
                thread_state_t::aborted);
        return;
    }

    if (cstate.is_polling() || cstate.is_waiting() ||
        cstate.is_waiting_notify())
    {
        msg_tag_t tag = get_tag();
        if (cstate.is_polling()) {
        }
        else
        {
            /* Thread is not yet receiving. Cancel the receive phase. */
            tag = tag.error_tag();
            this->remove_dependency();
        }

        // Set appropriate error code
        clear_exception_ipc();
        tag.set_error ();
        set_tag (tag);
        word_t err = ERR_IPC_CANCELED;
        set_error_code ((cstate.is_polling ()) ?
                       IPC_SND_ERROR (err) : IPC_RCV_ERROR (err));
        get_current_scheduler()->update_inactive_state(this, thread_state_t::aborted);
        return;
    }

    if (cstate.is_waiting_mutex()) {
        get_current_scheduler()->scheduler_lock();
        get_current_scheduler()->scheduler_unlock();
        get_current_scheduler()->update_inactive_state(this,
                thread_state_t::aborted);
        return;
    }
}

CONTINUATION_FUNCTION(handle_ipc_error)
{
    tcb_t * current = get_current_tcb();
    thread_state_t saved_state = current->get_saved_state();


    if (EXPECT_FALSE (current->resource_bits))
        current->resources.load (current);

    if (saved_state.is_running())
    {
        current->restore_state(3);
        current->return_from_user_interruption();
    }
    else
    {
        current->set_saved_state(thread_state_t::aborted); // sanity
        TCB_SYSDATA_IPC(current)->from_tid = capid_t::nilthread();
        current->return_from_ipc();
    }

    NOTREACHED();
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

CONTINUATION_FUNCTION(finish_kernel_ipc);

/**
 * Set new pager for a thread
 * @param tcb TCB of new pager
 */
void tcb_t::set_pager(tcb_t *tcb)
{
    if (get_pager()) {
        this->pager.remove_referenced(get_pager());
    }
    if (tcb) {
        this->pager.set_referenced(tcb);
    }
}

void tcb_t::set_exception_handler(tcb_t *tcb)
{
    if (get_exception_handler()) {
        this->exception_handler.remove_referenced(get_exception_handler());
    }
    if (tcb) {
        this->exception_handler.set_referenced(tcb);
    }
}

CONTINUATION_FUNCTION(finish_kernel_ipc)
{
    tcb_t * current = get_current_tcb();

    current->restore_state(3);
    ACTIVATE_CONTINUATION(TCB_SYSDATA_IPC(current)->ipc_continuation);
}

/**
 * Return from the SYS_THREAD_CONTROL syscall with a success return code.
 */
static
CONTINUATION_FUNCTION(finish_sys_thread_control)
{
    tcb_t * current = get_current_tcb();
    continuation_t cont =
        TCB_SYSDATA_THREAD_CTRL(current)->thread_ctrl_continuation;
    return_thread_control(1, cont);
}

static
CONTINUATION_FUNCTION(abort_sys_thread_control)
{
    tcb_t * current = get_current_tcb();

    continuation_t cont =
        TCB_SYSDATA_THREAD_CTRL(current)->thread_ctrl_continuation;
    return_thread_control(0, cont);
}

extern "C" void
start_post_syscall_callback(void)
{
    continuation_t cont = ASM_CONTINUATION;
    tcb_t * current = get_current_tcb();
    current->get_post_syscall_callback()(cont);

    ACTIVATE_CONTINUATION(cont);
}

#ifdef CONFIG_SCHEDULE_INHERITANCE
/**
 * Determine the highest priority syncpoint we currently hold.
 */
#if !defined(CONFIG_ENABLE_FASTPATHS) \
        || !defined(HAVE_TCB_CALC_EFFECTIVE_PRIO_FASTPATH)
prio_t
tcb_t::calc_effective_priority()
{
    prio_t max = this->base_prio;

    tcb_t * send_head = this->end_point.get_send_head();
    if (send_head != NULL && send_head->effective_prio > max) {
        max = send_head->effective_prio;
    }

    /* Threads attempting to receive from us... */
    tcb_t * recv_head = this->end_point.get_recv_head();
    if (recv_head != NULL && recv_head->effective_prio > max) {
        max = recv_head->effective_prio;
    }
    return max;
}
#endif
#endif

void
tcb_t::remove_dependency(void)
{
}

static bool set_thread_handlers(tcb_t *dest_tcb, capid_t pager_tid,
                                capid_t except_handler_tid)
{
    tcb_t *tcb = NULL;

    if (pager_tid.is_cap_type()) {
        tcb = get_current_clist()->lookup_thread_cap_unlocked(pager_tid);
        if (tcb == NULL) {
            return false;
        }
        dest_tcb->set_pager(tcb);
    } else {
        if (!pager_tid.is_nilthread() && !pager_tid.is_anythread()) {
            return false;
        }
    }

    if (except_handler_tid.is_cap_type()) {
        tcb = get_current_clist()->lookup_thread_cap_unlocked(except_handler_tid);
        if (tcb == NULL) {
            return false;
        }
        dest_tcb->set_exception_handler(tcb);
    } else {
        if (!except_handler_tid.is_nilthread() && !except_handler_tid.is_anythread()) {
            return false;
        }
    }

    return true;
}


SYS_THREAD_CONTROL (capid_t dest_tid, spaceid_t space_id,
                    capid_t unused, capid_t pager_tid,
                    capid_t except_handler_tid,
                    tc_resources_t thread_resources,
                    word_t utcb_address)
{
 
	LOCK_PRIVILEGED_SYSCALL();
    tcb_t *dest_tcb;
    continuation_t continuation = ASM_CONTINUATION;
    kmem_resource_t *kresource;

    /* Get current TCB, and set continuation data to 'thread control' mode. */
    tcb_t * current = get_current_tcb();

    /* Flag determining if a full schedule is required prior to the
     * completion of the system call. */
    bool need_to_schedule = false;

    current->sys_data.set_action(tcb_syscall_data_t::action_thread_control);

    if (EXPECT_FALSE(dest_tid.is_myself() || pager_tid.is_myself() ||
        except_handler_tid.is_myself()))
    {
        current->set_error_code (EINVALID_THREAD);
        goto error_out;
    }

    /* Check whether the caller has any resources. */
    if (EXPECT_FALSE(!is_kresourced_space(get_current_space())))
    {
        current->set_error_code (EINVALID_SPACE);
        goto error_out;
    }

    /* Check for valid target cap identifier */
    if (EXPECT_FALSE(!dest_tid.is_cap_type()))
    {
        current->set_error_code (EINVALID_THREAD);
        goto error_out;
    }

    kresource = get_current_kmem_resource();

    if (space_id.is_nilspace())
    {
        /* === Thread modification/deletion === */
modify_try_again:
        /* Ensure a valid thread (master) cap is used */
        dest_tcb = get_current_clist()->lookup_thread_cap_locked(dest_tid, true);
        if (EXPECT_FALSE(dest_tcb == NULL))
        {
            current->set_error_code(EINVALID_THREAD);
            goto error_out;
        }

        if (utcb_address == 0)
        {
            /* === Thread deletion === */
            if (EXPECT_FALSE(dest_tcb == current))
            {
                /* do not allow deletion of ourself */
                current->set_error_code (EINVALID_THREAD);
                dest_tcb->unlock_write();
                goto error_out;
            }
            else
            {
                /* ok, delete the thread */
                space_t *space = dest_tcb->get_space();

                if (!get_current_clist()->remove_thread_cap(dest_tid)) {
                    /* the cap was modified underneath us */
                    dest_tcb->unlock_write();
                    goto modify_try_again;
                }
                space->remove_tcb(dest_tcb);

                UNLOCK_PRIVILEGED_SYSCALL();
                dest_tcb->delete_tcb(kresource);
                LOCK_PRIVILEGED_SYSCALL();
                need_to_schedule = true;
            }
        } // deletion
        else
        {
            /* === Thread modification === */
            if (EXPECT_FALSE(utcb_address != ~0UL))
            {
                /* cannot modify UTCB address */
                current->set_error_code (EUTCB_AREA);
                dest_tcb->unlock_write();
                goto error_out;
            }

            /* setup thread helpers */
            if (!set_thread_handlers(dest_tcb, pager_tid, except_handler_tid)) {
                current->set_error_code(EINVALID_PARAM);
                dest_tcb->unlock_write();
                goto error_out;
            }

            if (EXPECT_FALSE(thread_resources.is_valid())) {
                /* initialize thread resources */
                if (!dest_tcb->resources.control(dest_tcb, thread_resources,
                                                 kresource)) {
                    /* error code set in resources.control */
                    dest_tcb->unlock_write();
                    goto error_out;
                }
            }

            dest_tcb->unlock_write();
        } // modification
    }
    else
    {
        /* === Thread creation === */
        /* Check thread-id range, 0 is a valid thread-id */
        if (EXPECT_FALSE(!get_current_clist()->is_valid(dest_tid))) {
            current->set_error_code(EINVALID_THREAD);
            goto error_out;
        }

        space_t *space = get_current_space()->lookup_space(space_id);
        /* check for valid space */
        if (EXPECT_FALSE(space == NULL))
        {
            current->set_error_code(EINVALID_SPACE);
            goto error_out;
        }

        /* Check for valid UTCB location */
        if (EXPECT_FALSE(!space->check_utcb_location(utcb_address)))
        {
            current->set_error_code (EUTCB_AREA);
            goto error_out;
        }

        /* Allocate TCB */
        dest_tcb = allocate_tcb(dest_tid, kresource);

        if (EXPECT_FALSE(!dest_tcb)) {
            current->set_error_code (ENO_MEM);
            goto error_out;
        }

        /* Create the thread. */
        dest_tcb->init();

        dest_tcb->set_utcb_location(utcb_address);
        bool UNUSED locked = dest_tcb->try_lock_write();
        /* Insert TCB into capability list */
        if (EXPECT_FALSE(!get_current_clist()->add_thread_cap(dest_tid, dest_tcb))) {
            current->set_error_code(EINVALID_THREAD);
            goto free_cap_error_out;
        }

        /* set the space for the tcb */
        dest_tcb->set_space (space);
        space->add_tcb (dest_tcb);

        /* setup thread helpers */
        if (!set_thread_handlers(dest_tcb, pager_tid, except_handler_tid)) {
            current->set_error_code(EINVALID_PARAM);
            goto free_tcb_error_out;
        }

        /* Threads start blocked on IPC */
        dest_tcb->sys_data.set_action(tcb_syscall_data_t::action_ipc);

        /* activate thread, which in turn allocates the UTCB */
        if (EXPECT_FALSE(!dest_tcb->activate(thread_startup,
                                             kresource)))
        {
            /* Error code is set in activate() (allocate_utcb - should be ENO_MEM) */
            goto free_tcb_error_out;
        }
        fake_wait_for_startup (dest_tcb);

        if (thread_resources.bits.valid) {
            /* initialize thread resources */
            if (EXPECT_FALSE(!dest_tcb->resources.control(
                                 dest_tcb, thread_resources,
                                 kresource)))
            {
                /* error code set in resources.control() */
                goto free_tcb_error_out;
            }
            need_to_schedule = true;
        }
        /* Set new thread's thread handle in MR0. */
        current->set_mr(0, threadhandle(dest_tcb->tcb_idx).get_raw());
        dest_tcb->unlock_write();
    }

    if (need_to_schedule) {
        UNLOCK_PRIVILEGED_SYSCALL();
        TCB_SYSDATA_THREAD_CTRL(current)->thread_ctrl_continuation = continuation;
        get_current_scheduler()->schedule(current, finish_sys_thread_control,
                scheduler_t::sched_default);
        NOTREACHED();
    }
    UNLOCK_PRIVILEGED_SYSCALL();
    return_thread_control(1, continuation);
    NOTREACHED();

free_tcb_error_out:
    /* thread activation failed: clean up TCB */
    if (get_current_clist()->remove_thread_cap(dest_tid)) {
        /* We have exclusive access to dest_tcb now */
        space_t *space = dest_tcb->get_space();

        space->remove_tcb(dest_tcb);
    } else {
    }

free_cap_error_out:
    dest_tcb->unlock_write();
    UNLOCK_PRIVILEGED_SYSCALL();
    dest_tcb->delete_tcb(kresource);
    LOCK_PRIVILEGED_SYSCALL();

error_out:
    /* Hit an error. Abort the system call. */

    if (need_to_schedule) {
        UNLOCK_PRIVILEGED_SYSCALL();
        TCB_SYSDATA_THREAD_CTRL(current)->thread_ctrl_continuation = continuation;
        get_current_scheduler()->schedule(current, abort_sys_thread_control,
                scheduler_t::sched_default);
        NOTREACHED();
    }
    UNLOCK_PRIVILEGED_SYSCALL();
    return_thread_control(0, continuation);
    NOTREACHED();
}
