/*
 * Description:   ARM926EJ coprocessor-15 definitions
 */


#ifndef _ARCH_ARM_ARM926EJ_SYSCON_H_
#define _ARCH_ARM_ARM926EJ_SYSCON_H_

#define CONFIG_ARM_V5

/* Coprocessor 15 */
#define CP15                    p15

/* Primary CP15 registers (CRn) */
#define C15_id                  c0
#define C15_control             c1
#define C15_ttbase              c2
#define C15_domain              c3
#define C15_fault_status        c5
#define C15_fault_addr          c6
#define C15_cache_con           c7
#define C15_tlb                 c8
#define C15_cache_lock          c9
#define C15_tlb_lock            c10
#define C15_pid                 c13
#define C15_breakpoints         c14
#define C15_coprocessor         c15

/* Default secondary register (CRm) */
#define C15_CRm_default         c0
/* Default opcode2 register (opcode2) */
#define C15_OP2_default         0

/* CP15 - Control Register */
#define C15_CONTROL_FIXED       0x50078
#define C15_CONTROL_M           0x0001                  /* Memory management enable     */
#define C15_CONTROL_A           0x0002                  /* Alignment fault enable       */
#define C15_CONTROL_C           0x0004                  /* Data cache enable            */
#define C15_CONTROL_B           0x0080                  /* Big endian enable            */
#define C15_CONTROL_S           0x0100                  /* System access checks in MMU  */
#define C15_CONTROL_R           0x0200                  /* ROM access checks in MMU     */
#define C15_CONTROL_I           0x1000                  /* Instruction cache enable     */
#define C15_CONTROL_X           0x2000                  /* Remap interrupt vector       */
#define C15_CONTROL_RR          0x4000                  /* Cache replacement 0 - rand, 1 - RR */
#define C15_CONTROL_L4          0x8000                  /* ARMv4 Compatibility - Thumb  */

#define     ARM_ENDIAN          0

#define C15_CONTROL_INIT        (C15_CONTROL_FIXED | ARM_ENDIAN)

#define C15_CONTROL_KERNEL      (C15_CONTROL_FIXED | C15_CONTROL_M | C15_CONTROL_C |    \
                                 ARM_ENDIAN |  \
                                 C15_CONTROL_I | C15_CONTROL_X)

#define _INS_(x) #x
#define STR(x) _INS_(x)
#define _OUTPUT(x)  : [x] "=r" (x)
#define _INPUT(x)  :: [x] "r" (x)

#define read_cp15_register(CRn, CRm, op2, ret)                        \
    __asm__ __volatile__ (                                            \
        "mrc    p15, 0, "_(ret)","STR(CRn)", "STR(CRm)","STR(op2)";"  \
        _OUTPUT(ret))

#define write_cp15_register(CRn, CRm, op2, val)                     \
{                                                                   \
    word_t v = (word_t)val;                                         \
    __asm__ __volatile__ (                                          \
        "mcr    p15, 0, "_(v)","STR(CRn)", "STR(CRm)","STR(op2)";"  \
    _INPUT(v));                                                     \
};

#endif /*_ARCH_ARM_ARM926EJ_SYSCON_H_*/
