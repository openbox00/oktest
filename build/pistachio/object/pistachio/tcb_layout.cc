

#include <l4.h>
#define BUILD_TCB_LAYOUT
#define O(w,x) (u32_t)((char*) &w.x - (char*)(&w))
#if defined(__GNUC__)
#define DEFINE(label, val)     __asm__ __volatile__ ("DEFINE " #label " %n0" : : "i" (-val));
#elif defined(__RVCT_GNU__)
#define DEFINE(label, val)     DEFINE label __cpp(val);
#endif
#include </home/asic/okl4_3.0/pistachio/include/tcb.h>
tcb_t foo;


#if defined(__RVCT_GNU__)
__asm
#endif
void make_offsets() {
DEFINE(OFS_TCB_UTCB_LOCATION, O(foo, utcb_location)) 
DEFINE(OFS_TCB_UTCB, O(foo, utcb)) 
DEFINE(OFS_TCB_SPACE, O(foo, space)) 
DEFINE(OFS_TCB_SPACE_ID, O(foo, space_id)) 
DEFINE(OFS_TCB_PAGE_DIRECTORY, O(foo, page_directory)) 
DEFINE(OFS_TCB_PAGER, O(foo, pager)) 
DEFINE(OFS_TCB_THREAD_LOCK, O(foo, thread_lock)) 
DEFINE(OFS_TCB_THREAD_STATE, O(foo, thread_state)) 
DEFINE(OFS_TCB_PARTNER, O(foo, partner)) 
DEFINE(OFS_TCB_END_POINT, O(foo, end_point)) 
DEFINE(OFS_TCB_WAITING_FOR, O(foo, waiting_for)) 
DEFINE(OFS_TCB_EXCEPTION_HANDLER, O(foo, exception_handler)) 
DEFINE(OFS_TCB_RESOURCE_BITS, O(foo, resource_bits)) 
DEFINE(OFS_TCB_CONT, O(foo, cont)) 
DEFINE(OFS_TCB_PREEMPTION_CONTINUATION, O(foo, preemption_continuation)) 
DEFINE(OFS_TCB_ARCH, O(foo, arch)) 
DEFINE(OFS_TCB_RUNTIME_FLAGS, O(foo, runtime_flags)) 
#define TCB_RF_SUSPENDED   0
#define TCB_RF_USER_ACCESS 1
DEFINE(OFS_TCB_POST_SYSCALL_CALLBACK, O(foo, post_syscall_callback)) 
DEFINE(OFS_TCB_READY_LIST, O(foo, ready_list)) 
DEFINE(OFS_TCB_BLOCKED_LIST, O(foo, blocked_list)) 
DEFINE(OFS_TCB_MUTEXES_HEAD, O(foo, mutexes_head)) 
#if defined(CONFIG_DEBUG)
DEFINE(OFS_TCB_PRESENT_LIST, O(foo, present_list)) 
#endif
DEFINE(OFS_TCB_BASE_PRIO, O(foo, base_prio)) 
DEFINE(OFS_TCB_EFFECTIVE_PRIO, O(foo, effective_prio)) 
#if (defined CONFIG_MDOMAINS)
DEFINE(OFS_TCB_XCPU_LIST, O(foo, xcpu_list)) 
DEFINE(OFS_TCB_XCPU_STATUS, O(foo, xcpu_status)) 
#endif
DEFINE(OFS_TCB_TIMESLICE_LENGTH, O(foo, timeslice_length)) 
DEFINE(OFS_TCB_CURRENT_TIMESLICE, O(foo, current_timeslice)) 
#if defined(CONFIG_MUNITS) && defined(CONFIG_CONTEXT_BITMASKS)
DEFINE(OFS_TCB_CONTEXT_BITMASK, O(foo, context_bitmask)) 
#endif
#if defined(CONFIG_MUNITS)
DEFINE(OFS_TCB_RESERVED, O(foo, reserved)) 
#endif
DEFINE(OFS_TCB_SAVED_PARTNER, O(foo, saved_partner)) 
DEFINE(OFS_TCB_SAVED_STATE, O(foo, saved_state)) 
DEFINE(OFS_TCB_RESOURCES, O(foo, resources)) 
DEFINE(OFS_TCB_THREAD_LIST, O(foo, thread_list)) 
#ifdef CONFIG_THREAD_NAMES
DEFINE(OFS_TCB_DEBUG_NAME, O(foo, debug_name)) 
#endif
DEFINE(OFS_TCB_SAVED_SENT_FROM, O(foo, saved_sent_from)) 
DEFINE(OFS_TCB_SYS_DATA, O(foo, sys_data)) 
#if (defined CONFIG_MDOMAINS) || (defined CONFIG_MUNITS)
DEFINE(OFS_TCB_XCPU_CONTINUATION, O(foo, xcpu_continuation)) 
#endif
DEFINE(OFS_TCB_TCB_IDX, O(foo, tcb_idx)) 
DEFINE(OFS_TCB_MASTER_CAP, O(foo, master_cap)) 
DEFINE(OFS_TCB_SENT_FROM, O(foo, sent_from)) 
DEFINE(OFS_TCB_IRQ_STACK, O(foo, irq_stack)) 
#if (defined CONFIG_L4_PROFILING)
DEFINE(OFS_TCB_PROFILE_DATA, O(foo, profile_data)) 
#endif

}
