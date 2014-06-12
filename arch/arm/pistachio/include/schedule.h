/*
 * Description:
 */

#ifndef __ARM__SCHEDULE_H__
#define __ARM__SCHEDULE_H__

#include <arch/config.h>
#include <soc/soc.h>
#include <cpu/cpu.h>
#include <tcb.h>

PURE INLINE tcb_t *
get_active_schedule(void)
{
    return get_arm_globals()->current_schedule;
}

INLINE void
set_active_schedule(tcb_t * tcb)
{
    get_arm_globals()->current_schedule = tcb;
}

INLINE void preempt_disable()
{
    tcb_t *current = get_current_tcb();
    arm_cpu::cli();
    current->disable_preempt_recover();
}
#endif /* __ARM__SCHEDULE_H__ */
