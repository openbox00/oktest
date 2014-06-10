/*
 * Description:   TCB
 */
#ifndef __TCB_H__
#define __TCB_H__

#include <debug.h>
#include <atomic_ops/atomic_ops.h>
#include <threadstate.h>
#include <space.h>
#include <resources.h>
#include <capid.h>
#include <tcb_syscall_data.h>
#include <syncpoint.h>
#include <endpoint.h>
#include <utcb.h>
#include <smallalloc.h>
#include <read_write_lock.h>

/* implementation specific functions */
#include <arch/ktcb.h>

class space_t;
class prio_queue_t;
class scheduler_t;
class mutex_t;
class kmem_resource_t;

/**
 * tcb_t: kernel thread control block
 */
class tcb_t
{
public:
    bool activate(void (*startup_func)(), kmem_resource_t *kresource);
    bool create_kernel_thread(utcb_t * utcb);
    void delete_tcb(kmem_resource_t *kresource);
    void cancel_ipcs();
    bool release_mutexes();

    bool migrate_to_domain(cpu_context_t context);

    void unwind(tcb_t *partner);
    void enqueue_present();
    void dequeue_present();
    void set_mutex_thread_handle(capid_t handle);
    bool check_utcb_location (void);
    void set_utcb_location (word_t location);
    word_t get_utcb_location();

    void set_error_code (word_t err);
    word_t get_error_code (void);
private:
    void set_state(thread_state_t state);
public:
    thread_state_t get_state();
    void initialise_state(thread_state_t state);
    arch_ktcb_t * get_arch();

    void save_state(word_t mrs = MAX_SAVED_MESSAGE_REGISTERS);
    void restore_state(word_t mrs = MAX_SAVED_MESSAGE_REGISTERS);
    bool grab();
    void release();
    bool is_grabbed_by_me();
    INLINE void set_waiting_for(syncpoint_t * syncpoint);
    void remove_dependency(void);
    INLINE endpoint_t * get_endpoint(void);
    INLINE void reserve(void);
    INLINE void unreserve(void);
    INLINE bool is_reserved(void);
    cpu_context_t get_context();
    void set_partner(tcb_t *partner);
    tcb_t * get_partner();
    bool is_partner_valid();

    msg_tag_t get_tag();
    void set_tag(msg_tag_t tag);
    word_t get_mr(word_t index);
    void set_mr(word_t index, word_t value);
    bool copy_mrs(tcb_t * dest, word_t start, word_t count);
    void do_ipc(tcb_t *to_tcb, tcb_t *from_tcb,
            continuation_t continuation) NORETURN;

    void send_pagefault_ipc(addr_t addr, addr_t ip, space_t::access_e access,
            continuation_t continuation) NORETURN;
    void return_from_ipc(void) NORETURN;
    void return_from_user_interruption(void) NORETURN;
    void notify(continuation_t continuation);
    addr_t get_user_ip();
    addr_t get_user_sp();
    void set_user_ip(addr_t ip);
    void set_user_sp(addr_t sp);
    void copy_user_regs(tcb_t *src);
    word_t set_tls(word_t *mr);
    void copy_mrs_to_regs(tcb_t *dest);
    void copy_regs_to_mrs(tcb_t *src);
    void copy(tcb_t *src);
    bool is_suspended();
    void set_suspended(bool new_state);
    void set_post_syscall_callback(callback_func_t func);
    callback_func_t get_post_syscall_callback();
    addr_t get_preempted_ip();
    void set_preempted_ip(addr_t ip);
    addr_t get_preempt_callback_ip();
    void set_sender_space(spaceid_t space_id);
    spaceid_t get_sender_space();
    spaceid_t get_space_id() const PURE;
    space_t * get_space() const PURE;
    void set_space(space_t * space);
    bool is_local_domain();
    bool is_local_unit();
    utcb_t * get_utcb() const PURE;
    void set_utcb(utcb_t *new_utcb);

public:
    void set_pager(tcb_t *tcb);
    void set_exception_handler(tcb_t *tcb);
    void set_user_handle(const word_t handle);
    void set_user_flags(const word_t flags);
    void set_acceptor(const acceptor_t acceptor);

    tcb_t * get_pager();
    tcb_t * get_exception_handler();

    word_t get_user_handle();
    word_t get_user_flags();
    u8_t get_cop_flags();
    word_t * get_reg_stack_bottom (void);
    acceptor_t get_acceptor();

    void set_exception_ipc(word_t);
    bool in_exception_ipc(void);
    void clear_exception_ipc(void);
    bool copy_exception_mrs_from_frame(tcb_t *dest);
    bool copy_exception_mrs_to_frame(tcb_t *dest);

    /* asynchronous notification */
    void clear_notify_bits();
    word_t add_notify_bits(const word_t bits);
    word_t sub_notify_bits(const word_t bits);
    void set_notify_mask(const word_t mask);
    void enable_preempt_recover(continuation_t continuation);
    void disable_preempt_recover();
    void set_user_access(continuation_t cont);
    void clear_user_access(void);
    bool user_access_enabled(void);
    continuation_t user_access_continuation(void);

    word_t get_notify_bits();
    word_t get_notify_mask();
    bool try_lock_read() { return true; };
    bool try_lock_write() { return true; };
    bool is_locked() { return true; };

    /* IPC Control. */
    bool has_ipc_restrictions(void);
    bool cannot_send_ipc(const tcb_t*);

public:
    void init(void);

    /* stack manipulation */
public:
    void init_stack();
private:
    void create_startup_stack(void (*func)());

    friend void make_offsets(); /* generates OFS_TCB_ stuff */

public:

    tcb_t *get_saved_partner (void) { return saved_partner; }
    void set_saved_partner (tcb_t *t) { saved_partner = t; }

    thread_state_t get_saved_state (void)
        { return saved_state; }
    void set_saved_state (thread_state_t s)
        { saved_state = s; }
    void set_saved_state (thread_state_t::thread_state_e s)
        { saved_state = (thread_state_t) s; }

    /* do not delete this STRUCT_START_MARKER */

    /** these have relatively static values here **/

    /* user location of utcb */
    word_t              utcb_location;
    /* kernel alias of utcb */
    utcb_t *            utcb;

private:
    /* space the thread belongs to */
    space_t *           space;
    /* cache of space's unique identifier. */
    spaceid_t           space_id;

public:
    /* cache of space's page directory */
    pgent_t *           page_directory;

private:
    /* thread's pager */
    ref_t               pager;

    /** thread state and frequently modified data **/
private:
    read_write_lock_t   thread_lock;
    thread_state_t      thread_state;
    tcb_t *             partner;

public:
    /** End-point for this thread's IPC operations. */
    endpoint_t          end_point;

private:
    /** Synchronisation point, if any, this thread is currently waiting on. */
    syncpoint_t *       waiting_for;

    ref_t               exception_handler;

public:
    resource_bits_t     resource_bits;
    continuation_t      cont;

    /* preemption continuation */
    continuation_t      preemption_continuation;

    arch_ktcb_t         arch;

    /**
     * @brief A series of 1 bit flags.
     *
     * Two currently in use - "suspended" and "user_access"
     *
     * Only ever use via accessor functions.
     * @note: A BITFIELDX won't work here due to problems in the generated
     *        file tcb_layout.cc
     */
    word_t runtime_flags;

#define TCB_RF_SUSPENDED   0
#define TCB_RF_USER_ACCESS 1

    callback_func_t     post_syscall_callback;

    ringlist_t<tcb_t>   ready_list;

    /**
     * List of threads currently blocked on 'waiting_for'
     * synchronisation point.
     */
    ringlist_t<tcb_t>   blocked_list;

    /** List of mutexes currently held by this thread. */
    mutex_t *           mutexes_head;

#if defined(CONFIG_DEBUG)
    ringlist_t<tcb_t>   present_list;
#endif

    /**
     * The priority of the thread, before taking into account interactions with
     * other threads. This value is used as starting point for calculating the
     * effective priority of a thread.
     */
    prio_t              base_prio;

    /**
     * The priority of the thread, after taking into account priority
     * inheritance from other threads. This is the priority that should be used
     * when determining which thread should be running at a particular point in
     * time.
     */
    prio_t              effective_prio;

#if (defined CONFIG_MDOMAINS)
    ringlist_t<tcb_t>   xcpu_list;
    word_t              xcpu_status;

#endif

public:

    /* scheduling */
    word_t              timeslice_length;
    word_t              current_timeslice;

#if defined(CONFIG_MUNITS) && defined(CONFIG_CONTEXT_BITMASKS)
    /* Only allow the thread to run on certain hardware units. */
    word_t              context_bitmask;
#endif

#if defined(CONFIG_MUNITS)
    /* Set to a positivbe value 'true' if this thread is reserved to be scheduled /
     * grabbed by another hardware unit. If so, the thread should not be
     * enqueued again. */
    okl4_atomic_word_t       reserved;
#endif

public:
    tcb_t *             saved_partner;
    thread_state_t      saved_state;
    thread_resources_t  resources;
    ringlist_t<tcb_t>   thread_list;
#ifdef CONFIG_THREAD_NAMES
    char                debug_name[MAX_DEBUG_NAME_LENGTH];
#endif
    capid_t             saved_sent_from;
public:
    tcb_syscall_data_t  sys_data;

#if (defined CONFIG_MDOMAINS) || (defined CONFIG_MUNITS)
    /* Mailbox walking continuation */
    continuation_t      xcpu_continuation;
#endif

    word_t              tcb_idx;
    cap_t *             master_cap;
    capid_t             sent_from;
    /* saved interrupt stack -> only used from interrupt assembly routine */
    word_t              irq_stack;

#if (defined CONFIG_L4_PROFILING)
    profile_thread_data_t profile_data;
#endif

private:
    /* do not delete this STRUCT_END_MARKER */

    /* class friends */
    friend void dump_tcb(tcb_t *);
    friend void handle_ipc_error (void);
    friend class thread_resources_t;
    friend class scheduler_t;
    friend void switch_to(tcb_t *, tcb_t *);
    friend void switch_from(tcb_t *, continuation_t);
    friend void set_running_and_enqueue(tcb_t * tcb);
};

/* union to allow allocation of kernel stacks */
union stack_t {
    u8_t pad[STACK_SIZE];
};

#define KTCB_SIZE   ((sizeof(tcb_t) + (KTCB_ALIGN-1)) & (~(KTCB_ALIGN-1)))

/*
 * Thread lookup
 */
#if defined(CONFIG_DEBUG)
/**
 * Translates a pointer within a TCB into a valid TCB pointer,
 * or NULL if the pointer is not valid.
 *
 * @param ptr  Pointer to somewhere in the TCB.
 * @returns    Pointer to the TCB.
 */
tcb_t * get_tcb(void * ptr);
#endif

tcb_t* lookup_tcb_by_handle_locked(word_t threadhandle);
tcb_t* acquire_read_lock_tcb(tcb_t *tcb, tcb_t *tcb_locked = NULL);

INLINE tcb_t* acquire_read_lock_current(tcb_t *current, tcb_t *tcb_locked = NULL)
{
    if (current == tcb_locked) {
        //current->lock_read_already_held();
    } else if (EXPECT_FALSE(!current->try_lock_read())) {
        return NULL;
    }
    return current;
}

/*
 * TCB allocation and freeing
 */
tcb_t* allocate_tcb(capid_t tid, kmem_resource_t *kresource);
void free_tcb(tcb_t *tcb);

void init_tcb_allocator(void);

/**********************************************************************
 *
 * Thread Context Switching
 *
 **********************************************************************/

/**
 * Mark that the given thread should be considered as no longer running.
 *
 * This function will return, and the kernel will continue to execute on
 * its current stack, but will do so without any concept of
 * 'get_current_thread()'.
 *
 * This function allows a thread to be released by the kernel, more work
 * performed, and then a new thread switched to using the 'switch_to'
 * call. On uniprocessor systems, this is generally unnecessary, but on
 * SMP/SMT systems this is required by the scheduler.
 *
 * The given continuation function will be were the current thread
 * next wakes up.
 *
 * @param current
 *     The currently running TCB that will be switched away from.
 *
 * @param continuation
 *     The continuation that 'current' should wake up at when
 *     it is next scheduled.
 */
void switch_from(tcb_t * current, continuation_t continuation);

/**
 * Perform a context switch to another thread. The kernel must have
 * already called 'switch_from' to disown the currently running thread.
 * Only the scheduler should need to use these functions.
 *
 * This is the most primitive control function. The continuation
 * argument is the point where the current thread will execute upon
 * being resumed.
 *
 * Also passed in is a thread whose schedule we will be running
 * under. If the system is using schedule-inheritance, this means
 * that 'dest' may run using another thread's schedule. This will
 * occur when another thread (with higher priority) is blocked
 * waiting for 'dest' to finish. If 'schedule' is the same as
 * 'dest', then the destination thread runs on its own priority.
 *
 * At the conclusion of this function, execution will pass to the
 * destination's continuation, stored in 'cont'.
 *
 * @param dest
 *     The tcb to switch to.
 *
 * @param schedule
 *     The tcb whose schedule we will start to use. May be NULL to
 *     indicate no change of schedule will take place.
 *
 * @param continuation
 *     The continuation to activate when the thread is resumed.
 */
void switch_to(tcb_t * dest, tcb_t * schedule) NORETURN;

/**********************************************************************
 *
 *                    Generic access functions
 *               Operations on thread ids and settings
 *
 **********************************************************************/
extern spinlock_t state_lock;

INLINE space_t * tcb_t::get_space() const
{
    return space;
}

INLINE spaceid_t tcb_t::get_space_id() const
{
    return space_id;
}

INLINE utcb_t * tcb_t::get_utcb() const
{
    return this->utcb;
}

INLINE void tcb_t::set_utcb(utcb_t *new_utcb)
{
    this->utcb = new_utcb;
}


/**********************************************************************
 *
 *                  Access functions
 *
 **********************************************************************/
INLINE bool tcb_t::grab()
{
    return true;
}

INLINE void tcb_t::release()
{
}

INLINE bool tcb_t::is_grabbed_by_me()
{
    return true;
}

INLINE cpu_context_t tcb_t::get_context()
{
    return 0;
}

INLINE void tcb_t::set_state(thread_state_t state)
{
    this->thread_state.state = state.state;
}

INLINE void tcb_t::initialise_state(thread_state_t state)
{
    this->thread_state.state = state.state;
}

INLINE thread_state_t tcb_t::get_state()
{
    return thread_state;
}

INLINE void tcb_t::set_partner(tcb_t *tcb)
{
    this->partner = tcb;
}

INLINE bool tcb_t::is_partner_valid()
{
#ifdef CONFIG_DEBUG
    return (word_t)this->partner != INVALID_RAW;
#else
    return true;
#endif
}

INLINE tcb_t * tcb_t::get_partner()
{
    return this->partner;
}

INLINE endpoint_t *
tcb_t::get_endpoint(void)
{
    return &(this->end_point);
}

INLINE void
tcb_t::set_waiting_for(syncpoint_t * syncpoint)
{
    this->waiting_for = syncpoint;
}
#if 0
INLINE syncpoint_t *
tcb_t::get_waiting_for(void)
{
    //return this->waiting_for;
}
#endif
/**
 * Get TCB of a thread's pager
 * @return      TCB of pager
 */
INLINE tcb_t * tcb_t::get_pager()
{
    return this->pager.get_tcb();
}

/**
 * Get TCB of a thread's exception handler
 * @return      TCB of exception handler
 */
INLINE tcb_t * tcb_t::get_exception_handler()
{
    return this->exception_handler.get_tcb();
}

/**
 * Get a thread's user-defined handle
 * @return      user-defined handle
 */
INLINE word_t tcb_t::get_user_handle()
{
    return get_utcb()->user_defined_handle;
}

/**
 * Set user-defined handle for a thread
 * @param handle        new value for user-defined handle
 */
INLINE void tcb_t::set_user_handle(const word_t handle)
{
    get_utcb()->user_defined_handle = handle;
}

/**
 * Set the IPC error code
 * @param err   new IPC error code
 */
INLINE void tcb_t::set_error_code(word_t err)
{
    get_utcb()->error_code = err;
}

/**
 * Get the IPC error code
 * @return      IPC error code
 */
INLINE word_t tcb_t::get_error_code(void)
{
    return get_utcb()->error_code;
}

/**
 * Get a thread's preemption flags
 * @return      preemption flags
 */
/*
INLINE preempt_flags_t tcb_t::get_preempt_flags (void)
{
    preempt_flags_t flags;
    flags.raw = get_utcb()->preempt_flags;
    return flags;
}
*/
/**
 * Set a thread's preemption flags
 * @param flags new preemption flags
 */
/*
INLINE void tcb_t::set_preempt_flags (preempt_flags_t flags)
{
    get_utcb()->preempt_flags = flags.raw;
}
*/
/**
 * Get a thread's coprocessor flags
 * @return      coprocessor flags
 */
INLINE u8_t tcb_t::get_cop_flags (void)
{
    return get_utcb()->cop_flags;
}

/**
 * clear the notify_bits
 */
INLINE void tcb_t::clear_notify_bits()
{
    (void) okl4_atomic_set(&get_utcb()->notify_bits, 0);
}

/**
 * add bits to the notify_word
 */
INLINE word_t tcb_t::add_notify_bits(const word_t bits)
{
    return okl4_atomic_or_return(&get_utcb()->notify_bits, bits);
}

/**
 * clear bits in the notify_word
 */
INLINE word_t tcb_t::sub_notify_bits(const word_t bits)
{
    word_t previous = okl4_atomic_read(&get_utcb()->notify_bits);
    okl4_atomic_and(&get_utcb()->notify_bits, ~(bits));
    return previous;
}

/**
 * read value of the notify_bits
 */
INLINE word_t tcb_t::get_notify_bits(void)
{
    return okl4_atomic_read(&get_utcb()->notify_bits);
}

/**
 * read value of the notify_mask
 */
INLINE word_t tcb_t::get_notify_mask(void)
{
    return get_utcb()->notify_mask;
}

/**
 * set the value of the notify_mask register
 * @param value value to set
 */
INLINE void tcb_t::set_notify_mask(const word_t mask)
{
    get_utcb()->notify_mask = mask;
}

/**
 * Get message tag
 * @returns message tag
 * The message tag will be read from message register 0
 */
INLINE msg_tag_t tcb_t::get_tag (void)
{
    msg_tag_t tag;
    tag.raw = get_mr(0);
    return tag;
}

/**
 * Set the message tag
 * @param tag   new message tag
 * The message tag will be stored in message register 0
 */
INLINE void tcb_t::set_tag (msg_tag_t tag)
{
    set_mr(0, tag.raw);
}

/**
 * set value of sender_space
 */
INLINE void tcb_t::set_sender_space(spaceid_t space_id)
{
    get_utcb()->sender_space = space_id;
}

/**
 * read value of sender_space
 */
INLINE spaceid_t tcb_t::get_sender_space()
{
    return get_utcb()->sender_space;
}

/**
 * enqueue a tcb into the present list
 * the present list primarily exists for debugging reasons, since task
 * manipulations now happen on a per-thread basis
 */
#ifdef CONFIG_DEBUG
extern tcb_t * global_present_list;
extern spinlock_t present_list_lock;
#endif

INLINE void tcb_t::enqueue_present()
{
#ifdef CONFIG_DEBUG
    present_list_lock.lock();
    ENQUEUE_LIST_TAIL(tcb_t, global_present_list, this, present_list);
    present_list_lock.unlock();
#endif
}

INLINE void tcb_t::dequeue_present()
{
#ifdef CONFIG_DEBUG
    present_list_lock.lock();
    DEQUEUE_LIST(tcb_t, global_present_list, this, present_list);
    present_list_lock.unlock();
#endif
}

INLINE void
tcb_t::reserve(void)
{
}

INLINE void
tcb_t::unreserve(void)
{
}

INLINE bool
tcb_t::is_reserved(void)
{
    return false;
}

INLINE arch_ktcb_t *tcb_t::get_arch()
{
    return &this->arch;
}

/* Migrating domains is meaningless in not MP systems */
#ifdef CONFIG_MDOMAINS
INLINE bool tcb_t::migrate_to_domain(cpu_context_t context)
{
    // update the directory cache on migration between processors
    if (space) {
        this->page_directory = space->pgent(0, context.domain);
    }

#error FIXME: Add stuff to migrate to a new domain here

    return false;
}
#endif

INLINE void init_idle_tcb()
{
    extern tcb_t* __idle_tcb[];
    int index = 0;
    __idle_tcb[index] = (tcb_t *)get_current_kmem_resource()->alloc(kmem_group_tcb, true);
    __idle_tcb[index]->tcb_idx = INVALID_RAW;
}

tcb_t * get_idle_tcb(cpu_context_t context) PURE;


INLINE tcb_t * get_idle_tcb(cpu_context_t context)
{
    extern tcb_t* __idle_tcb[];
    return __idle_tcb[0];
}

INLINE utcb_t * get_idle_utcb(cpu_context_t context)
{
    extern utcb_t __idle_utcb;
    return (utcb_t*)&__idle_utcb;
}

INLINE tcb_t * get_idle_tcb()
{
    return get_idle_tcb(0);
}

INLINE utcb_t * get_idle_utcb()
{
    return get_idle_utcb(0);
}

/*
 * include glue header file
 */
#include <arch/tcb.h>

/**********************************************************************
 *
 *             global thread management
 *
 **********************************************************************/

INLINE space_t * get_current_space(void) PURE;

INLINE space_t * get_current_space(void)
{
    return get_current_tcb()->get_space();
}

INLINE bool tcb_t::is_local_unit()
{
        return true;
}

INLINE bool tcb_t::is_local_domain()
{
    return true;
}

/**
 * Check UTCB location of thread is valid.  It is assumed that the
 * space of the thread is properly initialised.
 *
 * @return true if UTCB location is valid, false otherwise
 */
INLINE bool tcb_t::check_utcb_location ()
{
    return get_space()->check_utcb_location (get_utcb_location ());
}

/**********************************************************************
 *
 *                      user access fault functions
 *
 **********************************************************************/

INLINE void
tcb_t::set_user_access(continuation_t cont)
{
    /* Ensure that we don't already user access enabled. It would be
     * ideal if we could ensure that nobody else is using this field at
     * the present time (for instance, visible kernel preemption), but
     * unfortunately this field is left with stale data in it after a
     * (timer-based) preemption continuation takes place.  */
    this->runtime_flags |= (word_t)(1 << TCB_RF_USER_ACCESS);
    this->preemption_continuation = cont;

#if defined(ARCH_ENABLE_USER_ACCESS)
    ARCH_ENABLE_USER_ACCESS;
#endif

    okl4_atomic_compiler_barrier();
}

INLINE void
tcb_t::clear_user_access(void)
{
    okl4_atomic_compiler_barrier();

#if defined(ARCH_DISABLE_USER_ACCESS)
    ARCH_DISABLE_USER_ACCESS;
#endif
    this->runtime_flags &= ~((word_t)(1 << TCB_RF_USER_ACCESS));
    this->preemption_continuation = NULL;

    okl4_atomic_compiler_barrier();
}

INLINE bool
tcb_t::user_access_enabled(void)
{
    return this->runtime_flags & (1 << TCB_RF_USER_ACCESS);
}

INLINE continuation_t
tcb_t::user_access_continuation(void)
{
    return this->preemption_continuation;
}

/**********************************************************************
 *
 *                        notification functions
 *
 **********************************************************************/

void handle_ipc_error(void);

extern "C" void arm_return_from_notify0(void);
/**
 * register a function to be run next time thread is scheduled
 *
 * This function may not be called on a currently executing thread
 *
 * The function must be a continuation function. The thread must not
 * have a current continuation function set unless the function being
 * set is handle_ipc_error.
 *
 * Arguments to the function must be stored in the TCB
 *
 * This is not a control function - ie it will return normally, not by
 * activating the given continuation
 *
 * @param func notify procedure to invoke upon thread execution
 */
INLINE void
tcb_t::notify(continuation_t func)
{
    cont = (continuation_t)func;
}

INLINE bool
tcb_t::is_suspended()
{
    return this->runtime_flags & (1 << TCB_RF_SUSPENDED);
}

INLINE void
tcb_t::set_suspended(bool new_state)
{
    bool current = this->is_suspended();

    /* Ensure that the new state is different from the old state. */
    if (current) {
        this->runtime_flags &= ~((word_t)(1 << TCB_RF_SUSPENDED));
    } else {
        this->runtime_flags |= (word_t)(1 << TCB_RF_SUSPENDED);
    }
}

INLINE void
tcb_t::set_post_syscall_callback(callback_func_t func)
{
    post_syscall_callback = func;
}

INLINE callback_func_t
tcb_t::get_post_syscall_callback(void)
{
    return post_syscall_callback;
}

/**
 * adds a thread to the space
 * @param tcb pointer to thread control block
 */

INLINE void generic_space_t::add_tcb(tcb_t * tcb)
{
    thread_count++;
    spaces_list_lock.lock();
    ENQUEUE_LIST_TAIL(tcb_t, thread_list, tcb, thread_list);
    spaces_list_lock.unlock();
    /* Ensure TCB is in space's page directory */
    (void)sync_kernel_space(tcb);
}

/**
 * removes a thread from a space
 * @param tcb_t thread control block
 * @return true if it was the last thread
 */
INLINE void generic_space_t::remove_tcb(tcb_t * tcb)
{
    thread_count--;
    spaces_list_lock.lock();
    DEQUEUE_LIST(tcb_t, thread_list, tcb, thread_list);
    spaces_list_lock.unlock();
}

#endif /* !__TCB_H__ */
