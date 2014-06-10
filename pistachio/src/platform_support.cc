#include <tcb.h>

word_t kernel_api_version = SOC_API_VERSION;

extern "C" utcb_t *
kernel_get_utcb(tcb_h obj)
{
    return ((tcb_t *)obj)->get_utcb();
}
