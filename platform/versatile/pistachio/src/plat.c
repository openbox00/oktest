/*
 * Description:   ARM versatile platform specfic functions
 */

#include <soc/soc.h>
#include <soc/interface.h>
#include <soc.h>
#include <reg.h>
#include <interrupt.h>

word_t soc_api_version = SOC_API_VERSION;

word_t soc_do_platform_control(tcb_h current,
                               plat_control_t control,
                               word_t param1,
                               word_t param2,
                               word_t param3,
                               continuation_t cont)
{
    utcb_t *current_utcb = kernel_get_utcb(current);
    current_utcb->error_code = ENOT_IMPLEMENTED;
    return 0;
}

