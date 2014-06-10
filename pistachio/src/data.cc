/*
 * Description:   Kernel static datastructures
 */

#include <l4.h>
#include <tcb.h>
#include <utcb.h>
#include <schedule.h>
#include <clist.h>

/* We declare the global data that the kernel uses here, as the number needed will vary depending
 * upon the mp properties of the platform - should this be in arch or glue instead?
 */

ALIGNED(8) scheduler_t          __scheduler UNIT("cpulocal");
ALIGNED(8) space_t              __kernel_space_object;

ALIGNED(8)                      utcb_t          __idle_utcb;
ALIGNED(sizeof(stack_t))        stack_t         __stack UNIT("cpulocal.stack");

tcb_t* __idle_tcb[CONFIG_NUM_UNITS] UNIT("cpulocal.tcb");

/* these variables must not be in BSS so Elfweaver can modify it */
tcb_t** thread_handle_array = (tcb_t **)1;
word_t num_tcbs = 1;
word_t free_tcb_idx = 0;

/* The flag indicate we havn't set up current_tcb yet, the consequence of
 * this is get_current_space cause page fault.
 */
word_t pre_tcb_init = 1;
