/*
 * Description:   Various asm definitions for arm
 */
#include <l4.h>
#include <tcb.h>
#include <schedule.h>
     
#define MKASMSYM(sym, val)      __asm__ __volatile__ ("SYM " #sym " %n0" : : "i" (-(val)))

void mkasmsym() {

#define cpp_offsetof(type, field) ((unsigned) &(((type *) 4)->field) - 4)
MKASMSYM( OFS_TCB_ARCH_CONTEXT, cpp_offsetof(tcb_t, arch.context));

MKASMSYM( OFS_GLOBAL_CURRENT_TCB,      cpp_offsetof(arm_globals_t, current_tcb));

}
