#include <schedule.h>


ALIGNED(8) scheduler_t          __scheduler UNIT("cpulocal");
ALIGNED(8) space_t              __kernel_space_object;

ALIGNED(8)                      utcb_t          __idle_utcb;
ALIGNED(sizeof(stack_t))        stack_t         __stack UNIT("cpulocal.stack");

tcb_t* __idle_tcb[CONFIG_NUM_UNITS] UNIT("cpulocal.tcb");

tcb_t** thread_handle_array = (tcb_t **)1;
word_t num_tcbs = 1;
word_t free_tcb_idx = 0;
word_t pre_tcb_init = 1;
