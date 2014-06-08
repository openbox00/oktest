/*
 * Description:   Various asm definitions for arm
 */
#include <l4.h>
#include <mkasmsym.h>

#include <tcb.h>
#include <schedule.h>
#include <space.h>
#include <resources.h>

MKASMSYM_START  /* Do not remove */

MKASMSYM( TSTATE_RUNNING, (word_t) thread_state_t::running );
MKASMSYM( TSTATE_WAITING_FOREVER, (word_t) thread_state_t::waiting_forever );
MKASMSYM( TSTATE_WAITING_NOTIFY, (word_t) thread_state_t::waiting_notify );
MKASMSYM( TSTATE_POLLING, (word_t) thread_state_t::polling );
MKASMSYM( TSTATE_ABORTED, (word_t) thread_state_t::aborted );

#define cpp_offsetof(type, field) ((unsigned) &(((type *) 4)->field) - 4)

MKASMSYM( OFS_SPACE_DOMAIN, cpp_offsetof(space_t, domain) );
MKASMSYM( OFS_SPACE_DOMAIN_MASK, cpp_offsetof(space_t, domain_mask) );
MKASMSYM( OFS_SPACE_PID, cpp_offsetof(space_t, pid_vspace_raw) );
MKASMSYM( OFS_SPACE_CLIST, cpp_offsetof(space_t, clist) );

MKASMSYM( OFS_UTCB_ERROR_CODE, cpp_offsetof(utcb_t, error_code));
MKASMSYM( OFS_UTCB_MR0, cpp_offsetof(utcb_t, mr));
MKASMSYM( OFS_UTCB_ACCEPTOR, cpp_offsetof(utcb_t, acceptor));
MKASMSYM( OFS_UTCB_NOTIFY_BITS, cpp_offsetof(utcb_t, notify_bits));
MKASMSYM( OFS_UTCB_NOTIFY_MASK, cpp_offsetof(utcb_t, notify_mask));
MKASMSYM( OFS_UTCB_THREAD_WORD, cpp_offsetof(utcb_t, thread_word));
MKASMSYM( OFS_UTCB_SENDER_SPACE, cpp_offsetof(utcb_t, sender_space));
MKASMSYM( LOG2_SIZEOF_WORD,     (word_t) 2); /* log2(sizeof(word_t)) */

MKASMSYM( IPC_ERROR_NOT_ACCEPTED, IPC_SND_ERROR(ERR_IPC_NOT_ACCEPTED) );
MKASMSYM( IPC_ERROR_TAG, IPC_MR0_ERROR );

MKASMSYM( KIPC_RESOURCE_BIT, (word_t) (1ul << KIPC));
MKASMSYM( EXCEPTIONFP_RESOURCE_BIT, (word_t) (1ul << EXCEPTIONFP));
MKASMSYM( OFS_TCB_ARCH_CONTEXT, cpp_offsetof(tcb_t, arch.context));
MKASMSYM( OFS_TCB_ARCH_EXC_NUM, cpp_offsetof(tcb_t, arch.exc_num));

MKASMSYM( OFS_SCHED_INDEX_BITMAP, cpp_offsetof(scheduler_t, prio_queue.index_bitmap));
MKASMSYM( OFS_SCHED_PRIO_BITMAP, cpp_offsetof(scheduler_t, prio_queue.prio_bitmap));

/* Syncpoints */
MKASMSYM( OFS_SYNCPOINT_DONATEE,      cpp_offsetof(syncpoint_t, donatee));
MKASMSYM( OFS_SYNCPOINT_BLOCKED_HEAD, cpp_offsetof(syncpoint_t, blocked_head));

/* Endpoints */
MKASMSYM( OFS_ENDPOINT_SEND_QUEUE,    cpp_offsetof(endpoint_t, send_queue));
#if defined(CONFIG_SCHEDULE_INHERITANCE)
MKASMSYM( OFS_ENDPOINT_RECV_QUEUE,    cpp_offsetof(endpoint_t, recv_queue));
#endif

/* Ringlists */
MKASMSYM( OFS_RINGLIST_NEXT,          cpp_offsetof(ringlist_t<tcb_t>, next));
MKASMSYM( OFS_RINGLIST_PREV,          cpp_offsetof(ringlist_t<tcb_t>, prev));

/* Capabilities */
MKASMSYM( OFS_CLIST_MAX_ID,           cpp_offsetof(clist_t, max_id));
MKASMSYM( OFS_CLIST_ENTRIES,          cpp_offsetof(clist_t, entries));
MKASMSYM( OFS_CAP_RAW,                cpp_offsetof(cap_t, raw));
MKASMSYM( OFS_REF_OBJECT,             cpp_offsetof(ref_t, object));
MKASMSYM( LOG2_SIZEOF_CAP_T,          (word_t) 3); /* log2(sizeof(cap_t)) */

/* capid constants */
MKASMSYM( ASM_ANYTHREAD_RAW,        (word_t) ANYTHREAD_RAW);
MKASMSYM( ASM_NILTHREAD_RAW,        (word_t) NILTHREAD_RAW);
MKASMSYM( ASM_MYSELF_RAW,           (word_t) MYSELF_RAW);
MKASMSYM( ASM_INVALID_CAP_RAW,      (word_t) INVALID_RAW);
MKASMSYM( ASM_CAP_TYPE_IPC,         (word_t) cap_type_ipc);
MKASMSYM( ASM_CAP_TYPE_THREAD,      (word_t) cap_type_thread);

#undef current_domain
#undef current_domain_mask
#undef domain_dirty

MKASMSYM( OFS_GLOBAL_KSPACE,           cpp_offsetof(arm_globals_t, kernel_space));
MKASMSYM( OFS_GLOBAL_CURRENT_TCB,      cpp_offsetof(arm_globals_t, current_tcb));
MKASMSYM( OFS_GLOBAL_CURRENT_SCHEDULE, cpp_offsetof(arm_globals_t, current_schedule));
MKASMSYM( OFS_GLOBAL_CURRENT_CLIST,    cpp_offsetof(arm_globals_t, current_clist));
MKASMSYM( OFS_GLOBAL_RAM_PHYS,         cpp_offsetof(arm_globals_t, phys_addr_ram));
#if defined(CONFIG_ENABLE_FASS)
MKASMSYM( OFS_ARM_CPD,                 cpp_offsetof(arm_globals_t, cpd));
MKASMSYM( OFS_ARM_CURRENT_DOMAIN,      cpp_offsetof(arm_globals_t, current_domain));
MKASMSYM( OFS_ARM_CURRENT_DOMAIN_MASK, cpp_offsetof(arm_globals_t, current_domain_mask));
MKASMSYM( OFS_ARM_DOMAIN_DIRTY,        cpp_offsetof(arm_globals_t, domain_dirty));
#endif
MKASMSYM( ASM_KTCB_SIZE,            (word_t)KTCB_SIZE);

MKASMSYM( ASM_CAPID_INDEX_MASK,     (word_t) CAPID_INDEX_MASK);
MKASMSYM( ASM_CAP_OBJ_INDEX_MASK,   (word_t) CAP_OBJ_INDEX_MASK);

#undef cpp_offsetof

MKASMSYM_END    /* Do not remove */
