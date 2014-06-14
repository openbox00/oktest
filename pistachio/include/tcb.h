/*
 * Description:   TCB
 */
#ifndef __TCB_H__
#define __TCB_H__

#include <debug.h>
#include <atomic_ops/atomic_ops.h>
#include <threadstate.h>
#include <space.h>
#include <capid.h>
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
    thread_state_t get_state();
    bool grab();
    bool is_grabbed_by_me();
    cpu_context_t get_context();
    tcb_t * get_partner();
    word_t get_mr(word_t index);
    void set_mr(word_t index, word_t value);

    void notify(continuation_t continuation);
    addr_t get_user_ip();
    addr_t get_user_sp();
    void set_user_ip(addr_t ip);
    void set_user_sp(addr_t sp);
    spaceid_t get_space_id() const PURE;
    space_t * get_space() const PURE;
    void set_space(space_t * space);
    bool is_local_domain();
    bool is_local_unit();
    utcb_t * get_utcb() const PURE;

public:
    void init(void);
public:
    void init_stack();
private:
    void create_startup_stack(void (*func)());
    friend void make_offsets(); /* generates OFS_TCB_ stuff */
public:
    /* do not delete this STRUCT_START_MARKER */

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

public:
    continuation_t      cont;
    arch_ktcb_t         arch;
    callback_func_t     post_syscall_callback;
    ringlist_t<tcb_t>   ready_list;
    ringlist_t<tcb_t>   blocked_list;
    prio_t              base_prio;

    prio_t              effective_prio;
public:
    thread_state_t      saved_state;
    ringlist_t<tcb_t>   thread_list;
private:
    /* do not delete this STRUCT_END_MARKER */

    /* class friends */
};

union stack_t {
    u8_t pad[STACK_SIZE];
};

#define KTCB_SIZE   ((sizeof(tcb_t) + (KTCB_ALIGN-1)) & (~(KTCB_ALIGN-1)))

tcb_t* lookup_tcb_by_handle_locked(word_t threadhandle);
tcb_t* acquire_read_lock_tcb(tcb_t *tcb, tcb_t *tcb_locked = NULL);


tcb_t* allocate_tcb(capid_t tid, kmem_resource_t *kresource);

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

#include <arch/tcb.h>

INLINE space_t * get_current_space(void) PURE;

INLINE space_t * get_current_space(void)
{
    return get_current_tcb()->get_space();
}

INLINE void
tcb_t::notify(continuation_t func)
{
    cont = (continuation_t)func;
}

#endif /* !__TCB_H__ */
