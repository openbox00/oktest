/*
 * Description:   IPC declarations
 */
#ifndef __IPC_H__
#define __IPC_H__

class tcb_t;

/**
 * Message protocol tags
 */
#if defined(L4_32BIT)
#define IRQ_TAG                        0xfffffff0UL
#define PAGEFAULT_TAG                  0xffffffe0UL
#define PREEMPTION_TAG                 0xffffffd0UL
#elif defined(L4_64BIT)
#define IRQ_TAG                        0xfffffffffffffff0UL
#define PAGEFAULT_TAG                  0xffffffffffffffe0UL
#define PREEMPTION_TAG                 0xffffffffffffffd0UL
#else
#error Unkwown wordsize
#endif

/**
 * Error codes
 */
#define ERR_IPC_NOPARTNER               (1)
#define ERR_IPC_NON_EXISTING            (2)
#define ERR_IPC_CANCELED                (3)
#define ERR_IPC_MSG_OVERFLOW            (4)
#define ERR_IPC_NOT_ACCEPTED            (5)
#define ERR_IPC_ABORTED                 (7)

/**
 * Error encoding
 */
#define IPC_SND_ERROR(err)              ((err << 1) | 0)
#define IPC_RCV_ERROR(err)              ((err << 1) | 1)

/**
 * MR0 values
 */
#define IPC_MR0_REDIRECTED              (1 << 13)
#define IPC_MR0_XCPU                    (1 << 14)
#define IPC_MR0_ERROR                   (1 << 15)

class msg_tag_t
{
public:
    word_t get_label() { return send.label; }
    word_t get_untyped() { return send.untyped; }
    bool get_memcpy() { return send.memcpy; }

    void clear_flags() { raw &= ~(0xf << 12);}
    void clear_receive_flags() { raw &= ~(0xf << 12); }

    void set(word_t untyped, word_t label,
            bool send_blocks, bool recv_blocks)
        {
            this->raw = 0;
            this->send.untyped = untyped;
            this->send.label = label;
            this->send.rcvblock = recv_blocks;
            this->send.sndblock = send_blocks;
        }

    bool recv_blocks() { return send.rcvblock; }
    void set_recv_blocks(bool val = true) { send.rcvblock = val; }

    bool send_blocks() { return send.sndblock; }
    void set_send_blocks(bool val = true) { send.sndblock = val; }

    bool is_notify() { return send.notify; }

    bool is_error() { return recv.error; }
    void set_error() { recv.error = 1; }

    static msg_tag_t nil_tag()
        {
            msg_tag_t tag;
            tag.raw = 0;
            return tag;
        }

    static msg_tag_t error_tag()
        {
            msg_tag_t tag;
            tag.raw = 0;
            tag.set_error();
            return tag;
        }

    static msg_tag_t tag(word_t untyped, word_t label,
            bool send_blocks, bool recv_blocks)
        {
            msg_tag_t tag;
            tag.set(untyped, label, send_blocks, recv_blocks);
            return tag;
        }

    static msg_tag_t irq_tag()
        {
            /* this is an output parameter */
            return tag(0, IRQ_TAG, false, false);
        }

    static msg_tag_t notify_tag()
        {
            return tag (1, 0, false, false);
        }

    static msg_tag_t preemption_tag()
        {
            return tag (0, PREEMPTION_TAG, true, true);
        }

    static msg_tag_t pagefault_tag(bool read, bool write, bool exec)
        {
            return tag (2, (PAGEFAULT_TAG) |
                        (read  ? 1 << 2 : 0) |
                        (write ? 1 << 1 : 0) |
                        (exec  ? 1 << 0 : 0),
                        true, true);
        }
public:
    union {
        word_t raw;
        struct {
            BITFIELD7(word_t,
                      untyped           : 6,
                      __res             : 6,
                      memcpy            : 1,
                      notify            : 1,
                      rcvblock          : 1,
                      sndblock          : 1,
                      label             : BITS_WORD - 16);
        } send;
        struct {
            BITFIELD7(word_t,
                      untyped           : 6,
                      __res1            : 6,
                      __res2            : 1,
                      __res             : 1,
                      xcpu              : 1,
                      error             : 1,
                      label             : BITS_WORD - 16);
        } recv;
    };
};

INLINE msg_tag_t msgtag (word_t rawtag)
{
    msg_tag_t t;
    t.raw = rawtag;
    return t;
}

class acceptor_t
{
public:
    inline void clear()
        { this->raw = 0; }

    inline void operator = (word_t raw)
        { this->raw = raw; }

    inline bool accept_notify()
        { return x.notify; }

    static inline acceptor_t untyped_words()
        {
            acceptor_t x;
            x.raw = 0;
            return x;
        }
public:
    union {
        word_t raw;
        struct {
            BITFIELD3(word_t,
                      _res1             : 1,
                      notify            : 1,
                      _res2             : BITS_WORD - 2);
        } x;
    };
};

void ipc(tcb_t *to_tcb, tcb_t *from_tcb, word_t wait_type) NORETURN;

#endif /* !__IPC_H__ */
