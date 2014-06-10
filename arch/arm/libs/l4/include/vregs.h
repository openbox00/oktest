/** @file
 *  @brief UTCB (virtual register) layout for ARM.
 */

#ifndef __L4__ARM__VREGS_H__
#define __L4__ARM__VREGS_H__

#include <l4/cust/utcb.h>

/**
 *  Number of message registers.
 */
#define __L4_NUM_MRS                32

/**
 *  Number of thread words copied by kernel.
 */
#define __L4_THREAD_WORDS_COPIED    4

/** UTCB structure layout.
 *  WARNING: Keep inline with the TCR locations.
 */
#define UTCB_STRUCT struct {                                            \
        capid_t             mutex_thread_handle;     /* 0 */            \
        word_t              user_defined_handle;     /* 4 */            \
        u8_t                preempt_flags;           /* 8 */            \
        u8_t                cop_flags;               /* 9 */            \
        u8_t                UTCB_CUST0;              /* 10 */           \
        u8_t                UTCB_CUST1;              /* 11 */           \
        acceptor_t          acceptor;                /* 12 */           \
        word_t              notify_mask;             /* 16 */           \
        notify_bits_t       notify_bits;             /* 20 */           \
        word_t              processor_no;            /* 24 */           \
        word_t              error_code;              /* 28 */           \
        word_t              __unused_virtsender;     /* 32 */           \
        word_t              preempt_callback_ip;     /* 36 */           \
        word_t              preempted_ip;            /* 40 */           \
        word_t              share_fault_addr;        /* 44 */           \
        spaceid_t           sender_space;            /* 48 */           \
        u16_t               UTCB_CUST2;              /* 52 */           \
        u16_t               UTCB_CUST3;              /* 54 */           \
        /* Reserved for future kernel use.              56 .. 63  */    \
        word_t              kernel_reserved[__L4_TCR_RESERVED_NUM];     \
                                                                        \
        word_t              mr[__L4_NUM_MRS];        /* 64 .. 191 */    \
                                                                        \
        word_t              tls_word;                /* 192 */          \
        /* Reserved for future platform specifics use.  196 .. 215 */   \
        word_t              platform_reserved[__L4_TCR_PLATFORM_NUM];   \
        /* Thread private words.                        216 .. 255 */   \
        word_t              thread_word[__L4_TCR_USER_NUM];             \
    }


/** Location of TCRs within UTCB.
 *  WARNING: Keep inline with the actual structures layout.
 */

#define __L4_TCR_USER_NUM                       (10)
#define __L4_TCR_PLATFORM_NUM                   (5)
#define __L4_TCR_RESERVED_NUM                   (2)
/*
 * Thread local storage
 */
#define __L4_TCR_USER_OFFSET                    (54)    /* 54..63 */
#define __L4_TCR_PLATFORM_OFFSET                (49)    /* 49..53 */
#define __L4_TCR_PLAT_TLS                       (48)

#define __L4_TCR_MR_OFFSET                      (16)

#define __L4_TCR_IDL4_RESTORE_COUNT             (15)
#define __L4_TCR_KERNEL_RESERVED4               (15)    /* __L4_TCR_IDL4_RESTORE_COUNT
                                                         */
#define __L4_TCR_KERNEL_RESERVED3               (14)
#define __L4_TCR_KERNEL_RESERVED2               (13)
#define __L4_TCR_SENDER_SPACE                   (12)
#define __L4_TCR_KERNEL_RESERVED0               (11)

#define __L4_TCR_PREEMPTED_IP                   (10)
#define __L4_TCR_PREEMPT_CALLBACK_IP            (9)
#define __L4_TCR_VIRTUAL_ACTUAL_SENDER          (8)
#define __L4_TCR_ERROR_CODE                     (7)
#define __L4_TCR_PROCESSOR_NO                   (6)
#define __L4_TCR_NOTIFY_BITS                    (5)
#define __L4_TCR_NOTIFY_MASK                    (4)
#define __L4_TCR_ACCEPTOR                       (3)
#define __L4_TCR_COP_FLAGS                      (2)     /* Cop flags: byte 1 */
#define __L4_TCR_PREEMPT_FLAGS                  (2)     /* Preempt flags: byte
                                                         * 0 */
#define __L4_TCR_USER_DEFINED_HANDLE            (1)


#ifndef USER_UTCB_REF
#define USER_UTCB_REF           0xff000ff0
#endif


#if !defined(__ASSEMBLER__)

INLINE word_t *__L4_ARM_Utcb(void) CONST;

INLINE word_t *
__L4_ARM_Utcb(void)
{
    return *(word_t **)(USER_UTCB_REF);
}

INLINE word_t * L4_GetUtcbBase(void) CONST;
INLINE word_t *
L4_GetUtcbBase(void)
{
    return __L4_ARM_Utcb();
}

#endif

#include <l4/cust/vregs.h>

#endif /* __L4__ARM__VREGS_H__ */
