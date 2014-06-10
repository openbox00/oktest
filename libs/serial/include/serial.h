#if defined SERIAL_DRIVER

#include <driver/driver.h>
#include <driver/device.h>
#include <driver/stream.h>

#define NUM_PACKETS 20

/**
 * SERIAL_DRIVER is defined in the build system, and indicates the
 * device driver against which we are compiled.
 */
extern struct driver SERIAL_DRIVER;

struct serial_device {
    struct resource         *resources;

    struct device_interface *di;
    struct stream_interface *tx;
    struct stream_interface *rx;
    struct stream_pkt       *freelist;

    int                     irq;
};

#define SERIAL_INIT_UNINITIALISED  (-1)

struct stream_pkt *do_alloc_packet(struct serial_device *);
int free_packets(struct serial_device *serial_device,
        struct stream_interface *stream_interface);
size_t serial_write(const void *data, long int position,
        size_t count, void *handle);
size_t serial_read(void *data, long int position, size_t count,
        void *handle);
int resource_init(void);
int serial_init(void);

#endif
