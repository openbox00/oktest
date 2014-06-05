/*
 * Description:  INLINE reference handling functions
 */
#ifndef __REF_INLINE_H__
#define __REF_INLINE_H__

INLINE void
cap_reference_t::remove_reference(tcb_t *tcb, cap_t *cap)
{
    remove_reference(tcb->master_cap, cap);
}

INLINE void
cap_reference_t::invalidate_reference(tcb_t *tcb)
{
    invalidate_reference(tcb->master_cap);
    tcb->master_cap = NULL;
}

/**
 * Note, this is not safe - tcb is not locked. This is a general
 * problem since thread creation cannot use this if it tried
 * to aquire a lock and the thread was already locked.
 */
INLINE void
ref_t::set_referenced(tcb_t *tcb)
{
    this->object = tcb;
}

/**
 * Note, this is not safe - tcb is not locked. This is a general
 * problem since thread creation cannot use this if it tried
 * to aquire a lock and the thread was already locked.
 */
INLINE void
ref_t::remove_referenced(tcb_t *tcb)
{
}

#endif /* __REF_INLINE_H__ */
