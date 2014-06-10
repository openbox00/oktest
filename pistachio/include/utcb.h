/** \file
 *  \brief User thread control block (UTCB) definition.
 */
#ifndef __UTCB_H__
#define __UTCB_H__

#include <ipc.h>
#include <capid.h>
#include <spaceid.h>
 
#include <l4/arch/vregs.h> /* UTCB Layout. */
#include <atomic_ops/atomic_ops.h>

typedef okl4_atomic_word_t notify_bits_t;

// FIXME: Are the STRUCT_START/END_MARKERS still required? - AGW.
class utcb_t {
public:
    bool allocate(capid_t tid);
    void copy(utcb_t *src);
    void free();

public:
    /* do not delete this STRUCT_START_MARKER */
    UTCB_STRUCT;
    /* do not delete this STRUCT_END_MARKER */
};

INLINE void
utcb_t::copy(utcb_t *src)
{
    this->mutex_thread_handle = src->mutex_thread_handle;
    this->user_defined_handle = src->user_defined_handle;
    this->preempt_flags = src->preempt_flags;
    this->cop_flags = src->cop_flags;
    this->acceptor = src->acceptor;
    this->notify_mask = src->notify_mask;
    this->notify_bits = src->notify_bits;
    this->processor_no = src->processor_no;
    this->preempt_callback_ip = src->preempt_callback_ip;
    this->tls_word = src->tls_word;

    for (int i = 0; i < __L4_THREAD_WORDS_COPIED; i++) {
        this->thread_word[i] = src->thread_word[i];
    }
}


#endif /* !__UTCB_H__ */
