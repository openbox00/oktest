/*
 * Description:   Mapping implementation
 */

#include <tcb.h>
#include <syscalls.h>

SYS_MAP_CONTROL (spaceid_t space_id, word_t control)
{
    continuation_t continuation = ASM_CONTINUATION;
    return_map_control(1, continuation);

}
