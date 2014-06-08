#ifndef _DRIVER_DEVICE_OPS_H_
#define _DRIVER_DEVICE_OPS_H_

#define DEVICE_SUCCESS 1

#define DEVICE_NONE 0
#define DEVICE_ENABLED 1
#define DEVICE_DISABLED 2

#include <driver/device_if.h>

static inline struct device_interface *
setup_device_instance(struct driver *driver, void *data)
{
    struct device_interface *device_interface = data;
    device_interface->ops = *driver->ops;
    device_interface->device = data;
    return device_interface;
}

#endif /* _DRIVER_DRIVER_OPS_H_ */
