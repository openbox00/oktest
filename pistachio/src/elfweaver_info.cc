/*
 * Description:   Information for Elfweaver.
 */

#include <debug.h>
#include <init.h>
#include <l4.h>
#include <tcb.h>
#include <utcb.h>
#include <l4/elfweaver_info.h>

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
        L4_Info_Uncached,
        L4_Info_Cached,
        L4_Info_IOMemory,
        L4_Info_IOMemoryShared,
        L4_Info_WriteThrough,
        L4_Info_WriteBack,
        L4_Info_Shared,
        L4_Info_NonShared,
        L4_Info_Custom,
        L4_Info_Strong,
        L4_Info_Buffered
    }
};
