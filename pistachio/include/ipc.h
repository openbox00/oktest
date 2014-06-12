/*
 * Description:   IPC declarations
 */
#ifndef __IPC_H__
#define __IPC_H__

class tcb_t;
class msg_tag_t
{
public:
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


class acceptor_t
{
};



#endif /* !__IPC_H__ */
