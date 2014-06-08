#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <stdint.h>
#include <stddef.h>
#include <driver/mem_space.h>

#ifndef KENGE_L4_ROOTSERVER
#include <driver/bus_space.h>
#include <driver/timer_resource.h>
#endif

#include <driver/x86_ioport_space.h>

#define DEVICE_SUCCESS 1

#define DEVICE_NONE 0
#define DEVICE_ENABLED 1
#define DEVICE_DISABLED 2
#define DEVICE_ERROR 3
#define TIMER_EXPIRED 37

struct device_ops;

struct driver {
    const char *name;
    size_t size;
    struct device_ops *ops;
};

enum resource_type { NO_RESOURCE, BUS_RESOURCE, INTERRUPT_RESOURCE, MEMORY_RESOURCE, TIMER_RESOURCE, X86_IOPORT_RESOURCE };

struct resource;
struct resource {
    enum resource_type type;
    struct resource *next;
    union {
#ifndef KENGE_L4_ROOTSERVER
        bus_space_t bus;
#endif
        mem_space_t mem;
        x86_ioport_space_t x86_ioport;
#ifndef KENGE_L4_ROOTSERVER
        timer_resource_t timer;
#endif
        int interrupt;
    } resource;
};

struct generic_interface {
    int interface_type;
    void *device;
};

#endif /* _DRIVER_H_ */
