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
#include <utcb.h>
#include <smallalloc.h>

/* implementation specific functions */
#include <arch/ktcb.h>

class space_t;
class prio_queue_t;
class scheduler_t;
class kmem_resource_t;

/**
 * tcb_t: kernel thread control block
 */
class tcb_t
{
public:
    bool activate(void (*startup_func)(), kmem_resource_t *kresource);
    bool create_kernel_thread(utcb_t * utcb);


    void unwind(tcb_t *partner);
    void enqueue_present();
    void dequeue_present();
    bool check_utcb_location (void);
    void set_utcb_location (word_t location);
    word_t get_utcb_location();

public:
    thread_state_t get_state();
    arch_ktcb_t * get_arch();

    bool grab();
    bool is_grabbed_by_me();
    cpu_context_t get_context();
    tcb_t * get_partner();
    bool is_partner_valid();

    msg_tag_t get_tag();
    word_t get_mr(word_t index);
    void set_mr(word_t index, word_t value);
    bool copy_mrs(tcb_t * dest, word_t start, word_t count);

    void notify(continuation_t continuation);
    addr_t get_user_ip();
    addr_t get_user_sp();
    void set_user_ip(addr_t ip);
    void set_user_sp(addr_t sp);
    word_t set_tls(word_t *mr);
    void copy(tcb_t *src);
    bool is_suspended();
    callback_func_t get_post_syscall_callback();
    addr_t get_preempted_ip();
    addr_t get_preempt_callback_ip();
    spaceid_t get_sender_space();
    spaceid_t get_space_id() const PURE;
    space_t * get_space() const PURE;
    void set_space(space_t * space);
    bool is_local_domain();
    bool is_local_unit();
    utcb_t * get_utcb() const PURE;
    void set_utcb(utcb_t *new_utcb);

public:
    tcb_t * get_pager();
    tcb_t * get_exception_handler();

    word_t get_user_handle();
    word_t get_user_flags();
    u8_t get_cop_flags();
    word_t * get_reg_stack_bottom (void);
    acceptor_t get_acceptor();

    bool in_exception_ipc(void);
    bool copy_exception_mrs_from_frame(tcb_t *dest);
    bool copy_exception_mrs_to_frame(tcb_t *dest);

    /* asynchronous notification */
    word_t add_notify_bits(const word_t bits);
    word_t sub_notify_bits(const word_t bits);
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
    thread_state_t      thread_state;
    tcb_t *             partner;

public:
    resource_bits_t     resource_bits;
    continuation_t      cont;
    continuation_t      preemption_continuation;
    arch_ktcb_t         arch;
    word_t runtime_flags;

#define TCB_RF_SUSPENDED   0
#define TCB_RF_USER_ACCESS 1

    callback_func_t     post_syscall_callback;
    ringlist_t<tcb_t>   ready_list;
    ringlist_t<tcb_t>   blocked_list;
    prio_t              base_prio;

    prio_t              effective_prio;

public:

    /* scheduling */
    word_t              timeslice_length;
    word_t              current_timeslice;

public:
    tcb_t *             saved_partner;
    thread_state_t      saved_state;
    thread_resources_t  resources;
    ringlist_t<tcb_t>   thread_list;

    capid_t             saved_sent_from;
public:
    tcb_syscall_data_t  sys_data;

    word_t              tcb_idx;
    cap_t *             master_cap;
    capid_t             sent_from;
    /* saved interrupt stack -> only used from interrupt assembly routine */
    word_t              irq_stack;


private:
    /* do not delete this STRUCT_END_MARKER */

    /* class friends */
};

/* union to allow allocation of kernel stacks */
union stack_t {
    u8_t pad[STACK_SIZE];
};

#define KTCB_SIZE   ((sizeof(tcb_t) + (KTCB_ALIGN-1)) & (~(KTCB_ALIGN-1)))

tcb_t* lookup_tcb_by_handle_locked(word_t threadhandle);
tcb_t* acquire_read_lock_tcb(tcb_t *tcb, tcb_t *tcb_locked = NULL);

INLINE tcb_t* acquire_read_lock_current(tcb_t *current, tcb_t *tcb_locked = NULL)
{
    if (current == tcb_locked) {
    } else if (EXPECT_FALSE(!current->try_lock_read())) {
        return NULL;
    }
    return current;
}

tcb_t* allocate_tcb(capid_t tid, kmem_resource_t *kresource);

/**********************************************************************
 *
 * Thread Context Switching
 *
 **********************************************************************/

void switch_from(tcb_t * current, continuation_t continuation);

void switch_to(tcb_t * dest, tcb_t * schedule) NORETURN;

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

INLINE bool tcb_t::grab()
{
    return true;
}
INLINE bool tcb_t::is_grabbed_by_me()
{
    return true;
}

INLINE thread_state_t tcb_t::get_state()
{
    return thread_state;
}


INLINE word_t tcb_t::get_user_handle()
{
    return get_utcb()->user_defined_handle;
}

INLINE u8_t tcb_t::get_cop_flags (void)
{
    return get_utcb()->cop_flags;
}

/**
 * read value of the notify_mask
 */
INLINE word_t tcb_t::get_notify_mask(void)
{
    return get_utcb()->notify_mask;
}

INLINE msg_tag_t tcb_t::get_tag (void)
{
    msg_tag_t tag;
    tag.raw = get_mr(0);
    return tag;
}


/**
 * read value of sender_space
 */
INLINE spaceid_t tcb_t::get_sender_space()
{
    return get_utcb()->sender_space;
}

INLINE arch_ktcb_t *tcb_t::get_arch()
{
    return &this->arch;
}

INLINE void init_idle_tcb()
{
    extern tcb_t* __idle_tcb[];
    int index = 0;
    __idle_tcb[index] = (tcb_t *)get_current_kmem_resource()->alloc(kmem_group_tcb, true);
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
tcb_t::notify(continuation_t func)
{
    cont = (continuation_t)func;
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


extern "C" void arm_return_from_notify0(void);



INLINE void generic_space_t::add_tcb(tcb_t * tcb)
{
    thread_count++;
    ENQUEUE_LIST_TAIL(tcb_t, thread_list, tcb, thread_list);
}

INLINE void generic_space_t::remove_tcb(tcb_t * tcb)
{
    thread_count--;
    DEQUEUE_LIST(tcb_t, thread_list, tcb, thread_list);
}

#endif /* !__TCB_H__ */
