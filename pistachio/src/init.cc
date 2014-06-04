/*
 * Description: Generic L4 init code
 */
#include <l4.h>
#include <debug.h>

/**
 * Entry point from ASM into C kernel
 * Precondition: MMU and page tables are disabled
 * Warning: Do not use local variables in startup_system()
 */
#include <tcb.h>
#include <schedule.h>
#include <space.h>
#include <arch/memory.h>
#include <config.h>
#include <arch/hwspace.h>
#include <kernel/generic/lib.h>
#include <soc/soc.h>
#include <mutex.h>
#include <clist.h>

void idle_thread();
void init_all_threads();

void SECTION(SEC_INIT) create_idle_thread()
{
    bool r;
    r = get_idle_tcb()->create_kernel_thread(get_idle_utcb());
    r = get_idle_tcb()->grab();
    get_idle_tcb()->initialise_state(thread_state_t::running);
    get_idle_tcb()->effective_prio = -1;
    get_idle_tcb()->base_prio = -1;
    /* set idle thread timeslice to infinite */
    (void)get_current_scheduler()->do_schedule(get_idle_tcb(), 0, ~0UL, 0);
    get_idle_tcb()->release();
}

/**
 * Setup system-wide scheduling.
 */
static NORETURN void SECTION(SEC_INIT)
startup_scheduler()
{
    create_idle_thread();
    get_idle_tcb()->notify (init_all_threads);

    /* get the thing going - we should never return */
    get_current_scheduler()->start(get_current_context());

    NOTREACHED();
}

/**
 * Perform architecture independent system-wide initialisation.
 *
 * At this point, all per-architecture initialisation has been performed.
 *
 * This function does not return.
 */
NORETURN void SECTION(SEC_INIT)
generic_init(void)
{
    /* Startup the scheduler, and begin to schedule threads. Does not
     * return. */
    startup_scheduler();

    NOTREACHED();
}
