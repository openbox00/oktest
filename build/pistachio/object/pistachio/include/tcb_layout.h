/* @LICENCE("Open Kernel Labs, Inc.", "2014")@ */

    /* machine-generated file - do NOT edit */
#ifndef __TCB_LAYOUT__H__
#define __TCB_LAYOUT__H__

//#define BUILD_TCB_LAYOUT 1
    #define OFS_TCB_UTCB_LOCATION               0x00 /*    0 */
#define OFS_TCB_UTCB                        0x04 /*    4 */
#define OFS_TCB_SPACE                       0x08 /*    8 */
#define OFS_TCB_SPACE_ID                    0x0c /*   12 */
#define OFS_TCB_PAGE_DIRECTORY              0x10 /*   16 */
#define OFS_TCB_PAGER                       0x14 /*   20 */
#define OFS_TCB_THREAD_LOCK                 0x1c /*   28 */
#define OFS_TCB_THREAD_STATE                0x20 /*   32 */
#define OFS_TCB_PARTNER                     0x24 /*   36 */
#define OFS_TCB_END_POINT                   0x28 /*   40 */
#define OFS_TCB_WAITING_FOR                 0x38 /*   56 */
#define OFS_TCB_EXCEPTION_HANDLER           0x3c /*   60 */
#define OFS_TCB_RESOURCE_BITS               0x44 /*   68 */
#define OFS_TCB_CONT                        0x48 /*   72 */
#define OFS_TCB_PREEMPTION_CONTINUATION     0x4c /*   76 */
#define OFS_TCB_ARCH                        0x50 /*   80 */
#define OFS_TCB_RUNTIME_FLAGS               0xac /*  172 */
#define OFS_TCB_POST_SYSCALL_CALLBACK       0xb0 /*  176 */
#define OFS_TCB_READY_LIST                  0xb4 /*  180 */
#define OFS_TCB_BLOCKED_LIST                0xbc /*  188 */
#define OFS_TCB_MUTEXES_HEAD                0xc4 /*  196 */
#define OFS_TCB_BASE_PRIO                   0xc8 /*  200 */
#define OFS_TCB_EFFECTIVE_PRIO              0xcc /*  204 */
#define OFS_TCB_TIMESLICE_LENGTH            0xd0 /*  208 */
#define OFS_TCB_CURRENT_TIMESLICE           0xd4 /*  212 */
#define OFS_TCB_SAVED_PARTNER               0xd8 /*  216 */
#define OFS_TCB_SAVED_STATE                 0xdc /*  220 */
#define OFS_TCB_RESOURCES                   0xe0 /*  224 */
#define OFS_TCB_THREAD_LIST                 0xe4 /*  228 */
#define OFS_TCB_SAVED_SENT_FROM             0xec /*  236 */
#define OFS_TCB_SYS_DATA                    0xf0 /*  240 */
#define OFS_TCB_TCB_IDX                     0x124 /*  292 */
#define OFS_TCB_MASTER_CAP                  0x128 /*  296 */
#define OFS_TCB_SENT_FROM                   0x12c /*  300 */
#define OFS_TCB_IRQ_STACK                   0x130 /*  304 */

#endif /* __TCB_LAYOUT__H__ */
