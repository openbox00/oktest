/* These numbers MUST be multiple of 4 */
#define SYSCALL_ipc                 0x0
#define SYSCALL_thread_switch       0x4
#define SYSCALL_thread_control      0x8
#define SYSCALL_exchange_registers  0xc
#define SYSCALL_schedule            0x10
#define SYSCALL_map_control         0x14
#define SYSCALL_space_control       0x18
/* Unused syscall slot:             0x1c */
#define SYSCALL_cache_control       0x20
/* Unused syscall slot:             0x24 */
#define SYSCALL_lipc                0x28
#define SYSCALL_platform_control    0x2c
#define SYSCALL_space_switch        0x30
#define SYSCALL_mutex               0x34
#define SYSCALL_mutex_control       0x38
#define SYSCALL_interrupt_control   0x3c
#define SYSCALL_cap_control         0x40
#define SYSCALL_memory_copy         0x44
#define SYSCALL_last                0x44

#define L4_TRAP_KPUTC               0xa0
#define L4_TRAP_KGETC               0xa4
#define L4_TRAP_KGETC_NB            0xa8
#define L4_TRAP_KDEBUG              0xac
#define L4_TRAP_GETUTCB             0xb0
#define L4_TRAP_CKBR                0xe4
/* L4_TRAP_KIP  was 0xb4 */
#define L4_TRAP_KSET_OBJECT_NAME    0xb8
#define L4_TRAP_GETCOUNTER          0xbc
#define L4_TRAP_GETNUMTPS           0xc0
#define L4_TRAP_GETTPNAME           0xc4
#define L4_TRAP_TCCTRL              0xc8

#define L4_TRAP_PMN_READ            0xcc
#define L4_TRAP_PMN_WRITE           0xd0
#define L4_TRAP_PMN_OFL_READ        0xd4
#define L4_TRAP_PMN_OFL_WRITE       0xd8

#define L4_TRAP_GETTICK             0xe0
#define L4_TRAP_WBTEST              0xec

/* Backwards compatability for iguana interrupts */
#define L4_TRAP_BOUNCE_INTERRUPT    0xf0

#define SYSBASE                     0xffffff00
#define SWIBASE                     0x1400

#define SYSNUM(name)                (SYSBASE + SYSCALL_ ## name)
#define SWINUM(name)                (SWIBASE + SYSCALL_ ## name)
#define TRAPNUM(name)               (SYSBASE + name)
