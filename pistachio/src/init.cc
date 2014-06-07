#include <tcb.h>
#include <schedule.h>

CONTINUATION_FUNCTION(idle_thread);

CONTINUATION_FUNCTION(idle_thread)
{
    while (1) {
    }
}

extern word_t pre_tcb_init;

/**
 * Setup system-wide scheduling.
 */
NORETURN void SECTION(SEC_INIT)
generic_init(void)
{
    pre_tcb_init = 0;
    run_init_script(INIT_PHASE_OTHERS);

    /* Perform the first schedule, and become the idle-thread proper when we
     * once again regain control. */
    get_current_scheduler()->schedule(get_current_tcb(), idle_thread, scheduler_t::sched_default);

    NOTREACHED();
}

