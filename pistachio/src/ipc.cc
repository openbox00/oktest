/*
 * Description:   generic IPC path
 */
#include <tcb.h>
#include <schedule.h>

void ipc(tcb_t *to_tcb, tcb_t *from_tcb, word_t wait_type)
{

    tcb_t * current = get_current_tcb();
    scheduler_t * scheduler = get_current_scheduler();
    scheduler->context_switch(current, from_tcb, thread_state_t::waiting_forever,thread_state_t::running,
                        TCB_SYSDATA_IPC(current)->ipc_return_continuation);

}
