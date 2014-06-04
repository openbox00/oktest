/* machine-generated file - do NOT edit */
#ifndef __ASMSYMS_H__
#define __ASMSYMS_H__

#define TSTATE_RUNNING            0x2
#define TSTATE_WAITING_FOREVER    0xffffffff
#define TSTATE_WAITING_NOTIFY     0x5
#define TSTATE_POLLING            0xb
#define TSTATE_ABORTED            0xf
#define OFS_SPACE_DOMAIN          0x40
#define OFS_SPACE_DOMAIN_MASK     0x44
#define OFS_SPACE_PID             0x48
#define OFS_SPACE_CLIST           0x1c
#define OFS_UTCB_ERROR_CODE       0x1c
#define OFS_UTCB_MR0              0x40
#define OFS_UTCB_ACCEPTOR         0xc
#define OFS_UTCB_NOTIFY_BITS      0x14
#define OFS_UTCB_NOTIFY_MASK      0x10
#define OFS_UTCB_THREAD_WORD      0xd8
#define OFS_UTCB_SENDER_SPACE     0x30
#define LOG2_SIZEOF_WORD          0x2
#define IPC_ERROR_NOT_ACCEPTED    0xa
#define IPC_ERROR_TAG             0x8000
#define KIPC_RESOURCE_BIT         0x1
#define EXCEPTIONFP_RESOURCE_BIT  0x2
#define OFS_TCB_ARCH_CONTEXT      0x50
#define OFS_TCB_ARCH_EXC_NUM      0xa8
#define OFS_SCHED_INDEX_BITMAP    0x400
#define OFS_SCHED_PRIO_BITMAP     0x404
#define OFS_SYNCPOINT_DONATEE     0x0
#define OFS_SYNCPOINT_BLOCKED_HEAD 0x4
#define OFS_ENDPOINT_SEND_QUEUE   0x0
#define OFS_ENDPOINT_RECV_QUEUE   0x8
#define OFS_MUTEX_SYNC_POINT      0x0
#define OFS_MUTEX_HELD_LIST       0xc
#define OFS_RINGLIST_NEXT         0x0
#define OFS_RINGLIST_PREV         0x4
#define OFS_CLIST_MAX_ID          0x0
#define OFS_CLIST_ENTRIES         0xc
#define OFS_CAP_RAW               0x0
#define OFS_REF_OBJECT            0x0
#define LOG2_SIZEOF_CAP_T         0x3
#define ASM_ANYTHREAD_RAW         0xffffffff
#define ASM_NILTHREAD_RAW         0xfffffffc
#define ASM_MYSELF_RAW            0xfffffffd
#define ASM_INVALID_CAP_RAW       0xfffffff
#define ASM_CAP_TYPE_IPC          0x2
#define ASM_CAP_TYPE_THREAD       0x1
#define OFS_GLOBAL_KSPACE         0x8
#define OFS_GLOBAL_CURRENT_TCB    0x0
#define OFS_GLOBAL_CURRENT_SCHEDULE 0x20
#define OFS_GLOBAL_CURRENT_CLIST  0x4
#define OFS_GLOBAL_RAM_PHYS       0x24
#define OFS_ARM_CPD               0xc
#define OFS_ARM_CURRENT_DOMAIN    0x10
#define OFS_ARM_CURRENT_DOMAIN_MASK 0x14
#define OFS_ARM_DOMAIN_DIRTY      0x18
#define ASM_KTCB_SIZE             0x140
#define ASM_CAPID_INDEX_MASK      0xfffffff
#define ASM_CAP_OBJ_INDEX_MASK    0xfffffff

#endif /* __ASMSYMS_H__ */
