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
 * space control privileged system call
 * (note: the glue layer has to provide a return_space_control macro to
 * load the return values into the appropriate registers)
 * @param space_id address space specifier
 * @param control space control parameter
 * @param clist_id clist to associate with this space
 * @param utcb_area user thread control block area fpage
 * @param space_resources address spaces resources
 */
SYS_SPACE_CONTROL (spaceid_t space_id, word_t control, clistid_t clist_id, fpage_t utcb_area, word_t space_resources);

/**
 * platform control system call
 * (note: the glue layer has to provide a return_platform_control(result)
 * macro to load the return values into the appropriate registers)
 * @param control control word
 * @param param1
 * @param param2
 * @param param3
 */
SYS_PLATFORM_CONTROL (plat_control_t control, word_t param1,word_t param2, word_t param3);

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
