#ifndef __TCB_STATES_H__
#define __TCB_STATES_H__

#include <l4.h>
#include <kernel/generic/lib.h>
#include <arch/ktcb.h>

class tcb_t;

class tcb_syscall_data_t
{
public:
    /**
     * @brief What's the current action going on in the thread
     *
     * Used for validation during testing.  However, it may be useful to
     * leave it around for informational purposes.  Definitely if KDB
     * is available.
     */
    enum action_e {
        action_none,
        action_schedule,
        action_exregs,
        action_ipc,
        action_cache_control,
        action_map_control,
        action_thread_control,

        /* related to user memory access, See user_access_t  */
        action_mutex,
        action_remote_memcpy
    };

    /**
     * @brief Grouping of continuations and data by state.
     *
     */
    /**
     * @brief sys_schedule
     */
    typedef struct {
        tcb_t          *dest_tcb;
        continuation_t  schedule_continuation;
    } sched_t;

    /**
     * @brief sys_exchange_registers
     */
    typedef struct {
        tcb_t          *source;
        word_t          ctrl;
        word_t          new_control;
        addr_t          sp;
        addr_t          ip;
        word_t          flags;
        word_t          user_handle;
        tcb_t *         exreg_tcb;
        continuation_t  exreg_continuation;
    } exregs_t;

    /**
     * IPC in its various forms.
     */
    typedef struct {
        capid_t         to_tid;
        capid_t         from_tid;
        /* IPC copy */

        word_t          saved_notify_mask;
        word_t          saved_error;

        continuation_t  ipc_return_continuation;
        continuation_t  ipc_restart_continuation;
        continuation_t  ipc_continuation;
        continuation_t  do_ipc_continuation; //< brought in from arch areas
    } ipc_t;

    /**
     * Cache control data for restarting after a preemption
     */
    typedef struct {
#ifdef CACHE_HANDLE_FAULT
        addr_t          fault_address;
#endif
        word_t          op_index;   // current operation index
        //cache_control_t ctrl;       // cache_control control word
        word_t          op_offset;  // current operation offset
        spaceid_t       space_id;   // target space

        continuation_t cache_continuation;
    } cache_ctrl_t;

    /**
     * Remote memory copy data for recovering from preemption
     * and data aborts.
     */
    typedef struct {
        /* hooks for the remote memory copy system call */
        addr_t              copy_start;

        /* data stash for memory copy system call preemption */
        addr_t              src;
        addr_t              dest;
        addr_t              orig_src;
        addr_t              orig_dest;
        word_t              size;
        word_t              orig_size;
        word_t              remote_size;

        tcb_t *             from_tcb;
        tcb_t *             to_tcb;
        continuation_t      memory_copy_cont;
    } remote_memcpy_data_t;

    // true if a thread was scheduled on last call to schedule
    bool                scheduled;

    /**
     * ThreadControl system call continuation data.
     */
    typedef struct {
        continuation_t thread_ctrl_continuation;
    } thread_ctrl_t;

#if 0
    /**
     * Mutex system call continuation data.
     */
    typedef struct {
        continuation_t mutex_continuation;
        //mutex_t *      mutex;
        bool           uncontended_hybrid;
        word_t *       hybrid_user_state;
    } mutex_t;
#endif

    /**
     * User memory access
     *
     * State information for every system call involving user memory
     * access is nested here as a union member. Non-union member,
     * fault_address is updated in handle_pagefault(), which does not
     * care about the specific action.
     *
     * Note that is_user_access() must be updated when you add
     * another member.
     */
    typedef struct {
        addr_t              fault_address;
        union {
#ifdef CONFIG_REMOTE_MEMORY_COPY
            remote_memcpy_data_t        memcpy;
#endif
            //mutex_t                     mutex;
        };
    } user_access_t;


#ifdef CONFIG_MUNITS
    /* SMT notify state - only used on idle thread */
    word_t              notify_arg;
#endif

public:
#ifdef TCB_DATA_SANITY
    action_e state;
#endif

    union {
        sched_t       sched;
        exregs_t      exregs;
        ipc_t         ipc;
        cache_ctrl_t  cache;
        thread_ctrl_t thread_ctrl;
        user_access_t user_access;
    } vals;

public:
    void set_action(action_e val);

#ifdef TCB_DATA_SANITY
    sched_t *       get_sched_vals(void);
    exregs_t *      get_exregs_vals(void);
    ipc_t *         get_ipc_vals(void);
    cache_ctrl_t *  get_cache_vals(void);
    thread_ctrl_t * get_thread_ctrl_vals(void);
    bool            is_user_access(void);
    mutex_t *       get_mutex_vals(void);
    user_access_t*  get_user_access_vals(void);
    remote_memcpy_data_t* get_memcpy_vals(void);
#endif
};

INLINE void
tcb_syscall_data_t::set_action(tcb_syscall_data_t::action_e val)
{
#ifdef TCB_DATA_SANITY
    if (this->state != val) {
        this->state = val;
    }
#endif
}

#ifdef TCB_DATA_SANITY

#define TCB_SYSDATA_SCHED(tcb)          tcb->sys_data.get_sched_vals()
#define TCB_SYSDATA_EXREGS(tcb)         tcb->sys_data.get_exregs_vals()
#define TCB_SYSDATA_IPC(tcb)            tcb->sys_data.get_ipc_vals()
#define TCB_SYSDATA_CACHE(tcb)          tcb->sys_data.get_cache_vals()
#define TCB_SYSDATA_THREAD_CTRL(tcb)    tcb->sys_data.get_thread_ctrl_vals()
#define TCB_SYSDATA_MUTEX(tcb)          tcb->sys_data.get_mutex_vals()
#define TCB_SYSDATA_USER_ACCESS(tcb)    tcb->sys_data.get_user_access_vals()
#define TCB_SYSDATA_MEMCPY(tcb)         tcb->sys_data.get_memcpy_vals()

#else

#define TCB_SYSDATA_SCHED(tcb)          (&tcb->sys_data.vals.sched)
#define TCB_SYSDATA_EXREGS(tcb)         (&tcb->sys_data.vals.exregs)
#define TCB_SYSDATA_IPC(tcb)            (&tcb->sys_data.vals.ipc)
#define TCB_SYSDATA_CACHE(tcb)          (&tcb->sys_data.vals.cache)
#define TCB_SYSDATA_THREAD_CTRL(tcb)    (&tcb->sys_data.vals.thread_ctrl)
#define TCB_SYSDATA_USER_ACCESS(tcb)    (&tcb->sys_data.vals.user_access)
#define TCB_SYSDATA_MEMCPY(tcb)         (&tcb->sys_data.vals.user_access.memcpy)
#define TCB_SYSDATA_MUTEX(tcb)          (&tcb->sys_data.vals.user_access.mutex)

#endif

#ifdef TCB_DATA_SANITY

INLINE tcb_syscall_data_t::sched_t *
tcb_syscall_data_t::get_sched_vals(void)
{
    ASSERT(ALWAYS, this->state == action_schedule);
    return &(this->vals.sched);
}

INLINE tcb_syscall_data_t::exregs_t *
tcb_syscall_data_t::get_exregs_vals(void)
{
    ASSERT(ALWAYS, this->state == action_exregs);
    return &(this->vals.exregs);
}

INLINE tcb_syscall_data_t::ipc_t *
tcb_syscall_data_t::get_ipc_vals(void)
{
    if (this->state != action_ipc)
        TRACEF("Value is %d, not %d\n", this->state, action_ipc);
    ASSERT(ALWAYS, this->state == action_ipc);
    return &(this->vals.ipc);
}

INLINE tcb_syscall_data_t::cache_ctrl_t *
tcb_syscall_data_t::get_cache_vals(void)
{
    ASSERT(ALWAYS, this->state == action_cache_control);
    return &(this->vals.cache);
}

INLINE tcb_syscall_data_t::thread_ctrl_t *
tcb_syscall_data_t::get_thread_ctrl_vals(void)
{
    ASSERT(ALWAYS, this->state == action_thread_control);
    return &(this->vals.thread_ctrl);
}

INLINE bool
tcb_syscall_data_t::is_user_access(void)
{
    switch (this->state) {
    case action_remote_memcpy:
    case action_mutex:
#ifdef CACHE_HANDLE_FAULT
    case action_cache_control:
#endif
        return true;
    default:
        return false;
    }
}

INLINE tcb_syscall_data_t::mutex_t *
tcb_syscall_data_t::get_mutex_vals(void)
{
    ASSERT(ALWAYS, this->state == action_mutex);
    return &(this->vals.user_access.mutex);
}

INLINE tcb_syscall_data_t::user_access_t *
tcb_syscall_data_t::get_user_access_vals(void)
{
    ASSERT(ALWAYS, is_user_access());
    return &(this->vals.user_access);
}

#ifdef CONFIG_REMOTE_MEMORY_COPY

INLINE tcb_syscall_data_t::remote_memcpy_data_t *
tcb_syscall_data_t::get_memcpy_vals(void)
{
    ASSERT(ALWAYS, this->state == action_remote_memcpy);
    return &(this->vals.user_access.memcpy);
}
#endif

#endif /* TCB_DATA_SANITY */

#endif /* ! __TCB_STATES_H__ */
