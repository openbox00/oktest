/*
 * Description:   Information for Elfweaver.
 */

#include <init.h>
#include <l4.h>
#include <tcb.h>
#include <utcb.h>

SECTION(".elfweaver_info") extern const struct elfweaver_info weaver_info = {
    {
    },
    sizeof(utcb_t),
    ARCH_MAX_SPACES,
    __L4_VALID_HW_PAGE_PERMS,
    sizeof(int),
    sizeof(int),
    sizeof(space_t),
    sizeof(tcb_t),
    sizeof(int),
    sizeof(segment_list_t),
    SMALL_OBJECT_BLOCKSIZE,
    KMEM_CHUNKSIZE,
    PG_TOP_SIZE,
    {
    }
};
