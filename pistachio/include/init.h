/*
 * Description:   specification for initialization
 */
#ifndef __INIT_H__
#define __INIT_H__

/* FIXME: does this go here? */
#if defined(__APPLE_CC__)
#define SEC_INIT "__INIT, __init"
#define SEC_INIT_DATA "__INIT, __init.data"
#define SEC_INIT_CPU "__INIT, __init.cpu"
#else
#define SEC_INIT ".init"
#define SEC_INIT_DATA ".init.data"
#define SEC_INIT_CPU ".init.cpu"
#endif

#include <types.h>

/* The definitions to be filled in by elfweaver */
struct roinit {
    word_t init_script;
};

struct elfweaver_info_cache {
    u16_t mask;
    u16_t comp;
};

/* The definitions to be read and used by elfweaver */
struct elfweaver_info {
    u8_t l4_default_attribs[12]; /* we support 12 cache policies */
    word_t utcb_size;
    word_t arch_max_spaces;
    word_t page_rights_mask;
    word_t clist_size;
    word_t cap_size;
    word_t space_size;
    word_t tcb_size;
    word_t mutex_size;
    word_t segment_list_size;
    word_t small_object_blocksize;
    word_t kmem_chunksize;
    word_t pgtable_top_size;
    struct elfweaver_info_cache arch_cache_perms[11];
};

INLINE const struct roinit *
get_init_data(void)
{
    extern const struct roinit init_struct;
    return &init_struct;
}


enum {
    INIT_PHASE_FIRST_HEAP,
    INIT_PHASE_OTHERS,
    INIT_PHASE_MAX
};

/*
 * This function runs operations in 'init script' corresponding to the
 * current phase.  If it is called again, it runs operations for the
 * next phase.
 */
void run_init_script(word_t phase);

#endif /* !__INIT_H__ */
