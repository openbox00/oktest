/*
 * Description:   Assembler macros for GNU tools
 */
#ifndef __ARCH__ARM__ASM_GNU_H__
#define __ARCH__ARM__ASM_GNU_H__

//#include <cpu/syscon.h>

/* XXX: inline asm hacks */
#define _(x)                    "%["#x"]"

/* calls / branches */
#if defined(CONFIG_ARM_V5) || defined(CONFIG_ARM_V6) || (CONFIG_ARM_VER >= 6)

#if defined(ASSEMBLY)
    .macro jump reg
    bx      \reg
    .endm
    .macro call reg
    blx     \reg
    .endm
#else
#define jump                    bx
#define call                    blx
#endif

#else

#if defined(ASSEMBLY)
    .macro jump reg
    mov     pc, \reg
    .endm
    .macro call reg
    mov     lr, pc
    mov     pc, \reg
    .endm
#else
#define jump                    mov     pc,
#define call                    mov     lr, pc; mov     pc,
#endif

#endif

#if defined(ASSEMBLY)
/* ARM style directives */
#define ALIGN                   .balign
#define CODE16                  .force_thumb
#define DCDU                    .word
#define END
#define EXPORT                  .global
#define IMPORT                  .extern
#define LABEL(name)             name:
#define LTORG                   .ltorg
#define MACRO                   .macro
#define MEND                    .endm

#define BEGIN_PROC(name)                        \
    .global name;                               \
    .type   name,function;                      \
    .align;                                     \
name:

#define END_PROC(name)                          \
3999:                                           \
    .size   name,3999b - name;

/* Functions to generate symbols in the output file
 * with correct relocated address for debugging
 */
#define TRAPS_BEGIN_MARKER                      \
    .section .data.traps;                       \
    .balign 4096;                               \
    __vector_vaddr:

#define VECTOR_WORD(name)                       \
    .equ    vector_##name, (name - __vector_vaddr + 0xffff0000);        \
    .global vector_##name;                      \
    .type   vector_##name,object;               \
    .size   vector_##name,4;                    \
name:

#define BEGIN_PROC_TRAPS(name)                  \
    .global name;                               \
    .type   name,function;                      \
    .equ    vector_##name, (name - __vector_vaddr + 0xffff0000);        \
    .global vector_##name;                      \
    .type   vector_##name,function;             \
    .align;                                     \
name:

#define END_PROC_TRAPS(name)                    \
3999:                                           \
    .size   name,3999b - name;                  \
    .equ    .fend_vector_##name, (3999b - __vector_vaddr + 0xffff0000);  \
    .size   vector_##name,.fend_vector_##name - vector_##name;

#endif

#define CHECK_ARG(a, b)                         \
        "   .ifnc " a ", " b "  \n"             \
        "   .err                \n"             \
        "   .endif              \n"             \

#if CONFIG_ARM_VER >= 6
/* ARMv6 Convenience Macros */
#define r13_svc     #0x13
#define svc_mode    #0x13

#define r13_abt     #0x17
#define abt_mode    #0x17
#endif

#endif /* __ARCH__ARM__ASM_GNU_H__ */
