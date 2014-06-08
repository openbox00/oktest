#ifndef __L4__UTCB_H__
#define __L4__UTCB_H__

#include <l4/types.h>
#include <l4/arch/vregs.h>

/** If we are compiling for user level provide missing types.
 *  WARNING: Make sure these match the size of the L4 types!
 */
typedef L4_SpaceId_t spaceid_t;
typedef L4_ThreadId_t capid_t;
typedef word_t acceptor_t;
typedef word_t notify_bits_t;
typedef L4_Word8_t u8_t;
typedef L4_Word16_t u16_t;
typedef L4_Word32_t u32_t;

typedef UTCB_STRUCT utcb_t;


INLINE utcb_t *
utcb_base_get(void)
{
    return (utcb_t *)L4_GetUtcbBase();
}

/**
 *  Thread Control Registers.
 */

INLINE word_t
__L4_TCR_MyGlobalId(void)
{
    return L4_myselfconst.raw;
}

INLINE word_t
__L4_TCR_ProcessorNo(void)
{
    return utcb_base_get()->processor_no;
}

INLINE word_t
__L4_TCR_UserDefinedHandle(void)
{
    return utcb_base_get()->user_defined_handle;
}

INLINE void
__L4_TCR_Set_UserDefinedHandle(word_t w)
{
    utcb_base_get()->user_defined_handle = w;
}

INLINE word_t
__L4_TCR_ErrorCode(void)
{
    return utcb_base_get()->error_code;
}

INLINE word_t
__L4_TCR_Acceptor(void)
{
    return utcb_base_get()->acceptor;
}

INLINE void
__L4_TCR_Set_Acceptor(word_t w)
{
    utcb_base_get()->acceptor = w;
}

INLINE word_t
__L4_TCR_NotifyMask(void)
{
    return utcb_base_get()->notify_mask;
}

INLINE word_t
__L4_TCR_NotifyBits(void)
{
    return utcb_base_get()->notify_bits;
}

INLINE void
__L4_TCR_Set_NotifyMask(word_t w)
{
    utcb_base_get()->notify_mask = w;
}

INLINE void
__L4_TCR_Set_NotifyBits(word_t w)
{
     utcb_base_get()->notify_bits = w;
}

INLINE void
L4_Set_CopFlag(word_t n)
{
    utcb_base_get()->cop_flags |= (1 << n);
}

INLINE void
L4_Clr_CopFlag(word_t n)
{
    utcb_base_get()->cop_flags &= ~(1 << n);
}

INLINE word_t
L4_PreemptedIP(void)
{
    return utcb_base_get()->preempted_ip;
}

INLINE void
L4_Set_PreemptCallbackIP(word_t ip)
{
    utcb_base_get()->preempt_callback_ip = ip;
}

// FIXME: Specify the shift amounts as a define - AGW.
INLINE L4_Bool_t
L4_EnablePreemptionCallback(void)
{
    L4_Bool_t old = (utcb_base_get()->preempt_flags >> 5) & 1;

    utcb_base_get()->preempt_flags |= (1 << 5);
    return old;
}

// FIXME: Specify the shift amounts as a define - AGW.
INLINE L4_Bool_t
L4_DisablePreemptionCallback(void)
{
    L4_Bool_t old = (utcb_base_get()->preempt_flags >> 5) & 1;

    utcb_base_get()->preempt_flags &= ~(1 << 5);
    return old;
}

INLINE word_t
__L4_TCR_SenderSpace(void)
{
    return utcb_base_get()->sender_space.raw;
}

INLINE word_t
__L4_TCR_ThreadWord(word_t n)
{
    return utcb_base_get()->thread_word[n];
}

INLINE void
__L4_TCR_Set_ThreadWord(word_t n, word_t w)
{
    utcb_base_get()->thread_word[n] = w;
}

INLINE word_t
__L4_TCR_ThreadLocalStorage(void)
{
    return utcb_base_get()->tls_word;
}

INLINE void
__L4_TCR_Set_ThreadLocalStorage(word_t tls)
{
    utcb_base_get()->tls_word = tls;
}

INLINE word_t
__L4_TCR_PlatformReserved(int i)
{
    return utcb_base_get()->platform_reserved[i];
}

INLINE void
__L4_TCR_Set_PlatformReserved(int i, word_t val)
{
    utcb_base_get()->platform_reserved[i] = val;
}

/**
 *  Message Registers.
 */

INLINE void
L4_StoreMR(int i, word_t * w)
{
    *w = utcb_base_get()->mr[i];
}

INLINE void
L4_LoadMR(int i, word_t w)
{
    utcb_base_get()->mr[i] = w;
}

INLINE word_t *
L4_MRStart(void)
{
    return &(utcb_base_get()->mr[0]);
}

INLINE void
L4_StoreMRs(int i, int k, word_t * w)
{
    word_t * mr = utcb_base_get()->mr + i;
    
    while (k-- > 0) {
        *w++ = *mr++;
    }
}

INLINE void
L4_LoadMRs(int i, int k, word_t * w)
{
    word_t * mr = utcb_base_get()->mr + i;
    
    while (k-- > 0) {
        *mr++ = *w++;
    }
}


#endif /* !__L4__UTCB_H__ */
