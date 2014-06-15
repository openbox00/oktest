#ifndef __L4__ARM__VREGS_H__
#define __L4__ARM__VREGS_H__

#define __L4_NUM_MRS                32

#define UTCB_STRUCT struct {                                            \
        capid_t             mutex_thread_handle;     /* 0 */            \
        word_t              user_defined_handle;     /* 4 */            \
        u8_t                preempt_flags;           /* 8 */            \
        u8_t                cop_flags;               /* 9 */            \
        u8_t                UTCB_CUST0;              /* 10 */           \
        u8_t                UTCB_CUST1;              /* 11 */           \
        word_t          	acceptor;                /* 12 */           \
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
        word_t              kernel_reserved[2];     \
                                                                        \
        word_t              mr[__L4_NUM_MRS];        /* 64 .. 191 */    \
                                                                        \
        word_t              tls_word;                /* 192 */          \
        /* Reserved for future platform specifics use.  196 .. 215 */   \
        word_t              platform_reserved[5];   \
        /* Thread private words.                        216 .. 255 */   \
        word_t              thread_word[__L4_TCR_USER_NUM];             \
    }

#define __L4_TCR_USER_NUM                       (10)



#endif /* __L4__ARM__VREGS_H__ */
