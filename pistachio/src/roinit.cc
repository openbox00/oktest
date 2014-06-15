/*
 * Description:  Kernel initialization data
 */
#include <tcb.h>
#include <init.h>

extern const struct roinit init_struct;

#define L4_ROOTSERVER_VERSION 1

SECTION(".roinit") const struct roinit init_struct = {
    NULL
};

