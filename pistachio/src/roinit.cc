/*
 * Description:  Kernel initialization data
 */
#include <l4.h>
#include <tcb.h>
#include <clist.h>
#include <init.h>

extern const struct roinit init_struct;

#define L4_ROOTSERVER_VERSION 1

SECTION(".roinit") const struct roinit init_struct = {
    NULL
};

