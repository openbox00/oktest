/*
 * Description:   declaration of system calls
 */
#ifndef __SYSCALLS_H__
#define __SYSCALLS_H__

#include <fpage.h>
#include <capid.h>
#include <arch/syscalls.h>
#include <soc/soc.h>

class spaceid_t;
//class mutexid_t;
class clistid_t;

class tc_resources_t;
class irq_control_t;
class cap_control_t;

extern "C" {

/**
 * the ipc system call
 * sys_ipc returns using the special return functions return_ipc and
 * return_ipc_error.  These have to be provided by the glue layer
 * @param to_tid destination thread id
 * @param from_tid from specifier
 */
SYS_IPC (capid_t to_tid, capid_t from_tid);


/**
 * thread switch system call
 * @param dest_tid thread id to switch to
 */
SYS_THREAD_SWITCH (capid_t dest_tid);


/**
 * thread control privileged system call
 * @param dest_tid thread id of the destination thread
 * @param space_id space specifier
 * @param scheduler_tid thread id of the scheduler thread
 * @param pager_tid thread id of the pager thread
 * @param except_handler_tid thread id of the exception_handler thread
 * @param thread_resources architecture specific resource bits
 * @param utcb_location location of the UTCB
 */
SYS_THREAD_CONTROL (capid_t dest_tid, spaceid_t space_id,
                    capid_t scheduler_tid, capid_t pager_tid,
                    capid_t except_handler_tid,
                    tc_resources_t thread_resources,
                    word_t utcb_location);

/**
 * exchange registers system call
 * @param dest_tid thread id of the destination thread (always global)
 * @param control control word specifying the operations to perform
 * @param usp user stack pointer
 * @param uip user instruction pointer
 * @param uflags user flags
 * @param uhandle user defined handle
 */
SYS_EXCHANGE_REGISTERS (capid_t dest_tid, word_t control,
                        word_t usp, word_t uip, word_t uflags,
                        word_t uhandle);


/**
 * schedule system call
 * (note: the glue layer has to provide a return_schedule macro to load
 * the return values into the appropriate registers)
 * @param dest_tid thread id of the destination thread
 * @param ts_len timeslice length
 * @param hw_thread_bitmask allow user thread to run on set of hw threads
 * @param processor_control processor number the thread migrates to
 * @param prio new priority of the thread
 * @param flags schedule system calls control flags.
 */
SYS_SCHEDULE (capid_t dest_tid, word_t ts_len,
        word_t hw_thread_bitmsk, word_t processor_control, word_t prio,
        word_t flags);


/**
 * map control system call
 * @param space_id target address space specifier
 * @param control map control word specifying the number of
 *  mapitems to be acted on
 */
SYS_MAP_CONTROL (spaceid_t space_id, word_t control);


/**
 * space control privileged system call
 * (note: the glue layer has to provide a return_space_control macro to
 * load the return values into the appropriate registers)
 * @param space_id address space specifier
 * @param control space control parameter
 * @param clist_id clist to associate with this space
 * @param utcb_area user thread control block area fpage
 * @param space_resources address spaces resources
 */
SYS_SPACE_CONTROL (spaceid_t space_id, word_t control, clistid_t clist_id,
                   fpage_t utcb_area, word_t space_resources);

/**
 * cache control system call
 * @param space_id target address space specifier
 * @param control control word specifing the options to cache_control
 */
SYS_CACHE_CONTROL (spaceid_t space_id, word_t control);

/**
 * platform control system call
 * (note: the glue layer has to provide a return_platform_control(result)
 * macro to load the return values into the appropriate registers)
 * @param control control word
 * @param param1
 * @param param2
 * @param param3
 */
SYS_PLATFORM_CONTROL (plat_control_t control, word_t param1,
                      word_t param2, word_t param3);

/**
 * space switch system call
 * @param tid thread to modify
 * @param space_id the space to move the thread into
 * @param utcb_location the new utcb address of the thread
 */
SYS_SPACE_SWITCH(capid_t tid, spaceid_t space_id, word_t utcb_location);

/**
 * Mutex system call.
 *
 * @param mutex_id The identifier of the mutex to modify.
 * @param flags The flags identifying the type of mutex operation to perform.
 * @param state_p The virtual address of the hybrid mutex word.
 */
//SYS_MUTEX(mutexid_t mutex_id, word_t flags, word_t * state_p);

/**
 * Mutex control privileged system call
 *
 * @param mutex_id Target mutex identifier.
 * @param control Control flags for system call.
 */
//SYS_MUTEX_CONTROL(mutexid_t mutex_id, word_t control);

/**
 * interrupt control system call
 * @param tid thread to modify
 * @param space_id the space to move the thread into
 * @param utcb_location the new utcb address of the thread
 */
SYS_INTERRUPT_CONTROL(capid_t tid, irq_control_t control);

/**
 * capability control system call
 * @param clist capbility list to operate on
 * @param control control flags for cap_control
 * @param mrs arguments dependant on control flags
 */
SYS_CAP_CONTROL(clistid_t clist, cap_control_t control);

/**
 * memory copy system call
 *
 * @param space_id the space to move the thread into
 * @param utcb_location the new utcb address of the thread
 */
SYS_MEMORY_COPY(capid_t remote, word_t local, word_t size, 
                word_t direction);

} /* extern "C" */


/*********************************************************************
 *                 control register constants
 *********************************************************************/

#define EXREGS_CONTROL_HALT     (1 << 0)
#define EXREGS_CONTROL_RECV     (1 << 1)
#define EXREGS_CONTROL_SEND     (1 << 2)
#define EXREGS_CONTROL_SP       (1 << 3)
#define EXREGS_CONTROL_IP       (1 << 4)
#define EXREGS_CONTROL_FLAGS    (1 << 5)
#define EXREGS_CONTROL_UHANDLE  (1 << 6)
//#define EXREGS_CONTROL_PAGER    (1 << 7)
#define EXREGS_CONTROL_TLS      (1 << 7)
#define EXREGS_CONTROL_HALTFLAG (1 << 8)
#define EXREGS_CONTROL_DELIVER  (1 << 9)
#define EXREGS_CONTROL_COPY_REGS        (1 << 10)
#define EXREGS_CONTROL_SUSPEND_FLAG     (1 << 11)
#define EXREGS_CONTROL_REGS_TO_MRS      (1 << 12)
#define EXREGS_CONTROL_MRS_TO_REGS      (1 << 13)
#define EXREGS_CONTROL_THREAD_SHIFT     (14)

#define SPACE_CONTROL_NEW                   (1 << 0)
#define SPACE_CONTROL_DELETE                (1 << 1)
#define SPACE_CONTROL_RESOURCES             (1 << 2)
#define SPACE_CONTROL_SPACE_PAGER           (1 << 3)
#define SPACE_CONTROL_KRESOURCES_ACCESSIBLE (1 << 4)
#define SPACE_CONTROL_PRIO(x)               ((x) >> (BITS_WORD-8))

#define MUTEX_CONTROL_CREATE (1 << 0)
#define MUTEX_CONTROL_DELETE (1 << 1)

#include <kernel/errors.h>

#endif /* !__SYSCALLS_H__ */
