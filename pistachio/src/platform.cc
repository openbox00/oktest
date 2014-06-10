#include <l4.h>
#include <arch/syscalls.h>
#include <soc/soc.h>
#include <tcb.h>

SYS_PLATFORM_CONTROL(plat_control_t control, word_t param1,
                     word_t param2, word_t param3)
{
    continuation_t continuation = ASM_CONTINUATION;
    tcb_t *current = get_current_tcb();
    word_t ret;
    ret = soc_do_platform_control((tcb_h)current, control, param1, param2, param3, continuation);
    return_platform_control(ret, continuation);
}

