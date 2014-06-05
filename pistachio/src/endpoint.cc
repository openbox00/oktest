#include <l4.h>
#include <tcb.h>
#include <schedule.h>

void
endpoint_t::init(tcb_t * tcb)
{
    //his->send_queue.init(tcb);

#if defined(CONFIG_SCHEDULE_INHERITANCE)

    //this->recv_queue.init(tcb);

#endif
}

void
endpoint_t::enqueue_send(tcb_t * tcb)
{
    scheduler_t * scheduler = get_current_scheduler();

    scheduler->scheduler_lock();
    //this->send_queue.block(tcb);
    scheduler->scheduler_unlock();
}

void
endpoint_t::dequeue_send(tcb_t * tcb)
{
    scheduler_t * scheduler = get_current_scheduler();

    scheduler->scheduler_lock();
    //this->send_queue.unblock(tcb);
    scheduler->scheduler_unlock();
}

void
endpoint_t::enqueue_recv(tcb_t * tcb)
{
#if defined(CONFIG_SCHEDULE_INHERITANCE)

    scheduler_t * scheduler = get_current_scheduler();

    scheduler->scheduler_lock();
    //this->recv_queue.block(tcb);
    scheduler->scheduler_unlock();

#endif
}

void
endpoint_t::dequeue_recv(tcb_t * tcb)
{
#if defined(CONFIG_SCHEDULE_INHERITANCE)

    scheduler_t * scheduler = get_current_scheduler();

    scheduler->scheduler_lock();
    //this->recv_queue.unblock(tcb);
    scheduler->scheduler_unlock();

#endif
}
