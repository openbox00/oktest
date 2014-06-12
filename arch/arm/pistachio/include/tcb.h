/*
 * Description:   ARM TCB handling functions
 */
#ifndef __ARM__TCB_H__
#define __ARM__TCB_H__

#include <kernel/arch/asm.h>
#include <kernel/arch/continuation.h>

extern stack_t __stack;

PURE INLINE tcb_t *
get_current_tcb (void)
{
    return get_arm_globals()->current_tcb;
}


/* include ARM version specific implementations */
#include <arch/ver/tcb.h>

INLINE word_t tcb_t::get_mr(word_t index)
{
    return get_utcb()->mr[index];
}

INLINE void tcb_t::set_mr(word_t index, word_t value)
{
    get_utcb()->mr[index] = value;
}

INLINE acceptor_t tcb_t::get_acceptor(void)
{
    return get_utcb()->acceptor;
}

INLINE bool tcb_t::in_exception_ipc(void)
{
    return (resource_bits & (1UL << (word_t)EXCEPTIONFP)) || (arch.exc_num != 0);
}

INLINE void tcb_t::set_space(space_t * new_space)
{
    this->space = new_space;

    if (EXPECT_TRUE(new_space)) {
        this->space_id = new_space->get_space_id();
        this->page_directory = new_space->pgent(0);
    } else {
        this->space_id = spaceid_t::nilspace();
        this->page_directory = NULL;
    }
}

INLINE void tcb_t::init_stack()
{
    /* Clear the exception context */
    arm_irq_context_t *context = &arch.context;
    word_t *t, *end;
    word_t size = sizeof(arch.context);

    /* clear whole context */
    t = (word_t*)context;
    end = (word_t*)((word_t)context + (size & ~(15)));

    do {
        *t++ = 0;
        *t++ = 0;
        *t++ = 0;
        *t++ = 0;
    } while (t < end);

    while (t < (word_t*)(context+1))
        *t++ = 0;
    context->pc = 1;    /* Make it look like an exception context. */
}

INLINE addr_t tcb_t::get_user_sp()
{
    arm_irq_context_t * context = &arch.context;
    return (addr_t) (context)->sp;
}

INLINE void tcb_t::set_user_sp(addr_t sp)
{
    arm_irq_context_t *context = &arch.context;
    context->sp = (word_t)sp;
}

INLINE word_t tcb_t::get_utcb_location()
{
    return (word_t)this->utcb_location;
}

INLINE void tcb_t::set_utcb_location(word_t location)
{
    utcb_location = location;
}

INLINE addr_t tcb_t::get_preempt_callback_ip()
{
    return (addr_t)get_utcb()->preempt_callback_ip;
}

#endif /* !__ARM__TCB_H__ */
