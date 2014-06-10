#include <stdlib.h>
#include <string.h>
#include <okl4/env.h>
#include <serial/serial.h>

static int8_t serial_init_result = SERIAL_INIT_UNINITIALISED;

struct serial_device serial_device;
/* Hardware resources.*/
struct resource      resources[8];
/* Data packets passed to and from driver. */
struct stream_pkt    stream_packet[NUM_PACKETS];

/* Pop a packet off the free list for the underlying device */
struct stream_pkt *
do_alloc_packet(struct serial_device *serial_device)
{
    struct stream_pkt *retval;
    if (serial_device->freelist == NULL) {
        return NULL;
    }
    retval = serial_device->freelist;
    serial_device->freelist = retval->next;
    return retval;
}

int
free_packets(struct serial_device *serial_device,
        struct stream_interface *stream_interface)
{
    int count = 0;

    while (stream_interface->completed_start) {
        struct stream_pkt *packet;
        struct stream_pkt *next;

        packet = stream_interface->completed_start;
        next = packet->next;

        /* Add to the freelist */
        packet->next = serial_device->freelist;
        serial_device->freelist = packet;

        stream_interface->completed_start = next;
        count++;
    }
    stream_interface->completed_end = NULL;

    return count;
}

size_t
serial_write(const void *data, long int position, size_t count, void *handle)
{
    struct stream_pkt *packet;
    char *cr_data;
    size_t cr_count;
    int cr_added = 0;

    free_packets(&serial_device, serial_device.tx);

    /* Obtain a packet from the free list. */
    packet = do_alloc_packet(&serial_device);
    if (packet == NULL) {
        return 0;
    }

    /* Add carriage return if the last character is '\n' */
    if (((char *)data)[count - 1] == '\n') {
        cr_count = count + 1;
        cr_data = malloc(cr_count);
        if (cr_data == NULL) {
            return 0;
        }
        cr_data[cr_count - 2] = '\r';
        cr_data[cr_count - 1] = '\n';
        cr_added = 1;
    }
    else {
        cr_count = count;
        cr_data = (char *)data;
    }

    /* Set the attributes of this packet. */
    packet->data = (uint8_t *)cr_data;
    packet->length = cr_count;
    packet->xferred = 0;
    packet->ref = NULL;

    stream_add_stream_pkt(serial_device.tx, packet);

    /* poll device until all data is sent. */
    while ((stream_get_head(serial_device.tx) != NULL)) {
        device_poll(serial_device.di);
    }

    /* If we allocated cr_data then free */
    if (cr_added) {
        free(cr_data);
    }

    return count;
}

size_t
serial_read(void *data, long int position, size_t count, void *handle)
{
    struct stream_pkt *packet;

    free_packets(&serial_device, serial_device.rx);

    packet = do_alloc_packet(&serial_device);
    if (packet == NULL) {
        return 0;
    }

    packet->data = (uint8_t *)data;
    packet->length = count;
    packet->xferred = 0;
    packet->ref = NULL;

    stream_add_stream_pkt(serial_device.rx, packet);

    /* poll device until we read a packet. */
    while ((stream_get_head(serial_device.rx) == NULL)) {
        device_poll(serial_device.di);
    }

    return count;
}

int
resource_init(void)
{
    int r = 0;
    okl4_env_segment_t *serial_seg;

    resources[r].type = MEMORY_RESOURCE;
    serial_seg = okl4_env_get_segment("MAIN_SERIAL_MEM0");

    if (serial_seg) {
        resources[r++].resource.mem = (mem_space_t)serial_seg->virt_addr;
    }
    /* Request interrupt? ... no, polling implementation. */

    for (; r < 8; r++) {
        resources[r].type = NO_RESOURCE;
    }

    return 0;
}

int
serial_init(void)
{
    void *device = NULL;
    int i;

    /* If already initialized, return immediately */
    if (serial_init_result != SERIAL_INIT_UNINITIALISED) {
        return serial_init_result;
    }

    if (resource_init() != 0) {
        /* Error. */
        return (serial_init_result = 1);
    }

    device = malloc(SERIAL_DRIVER.size);
    if (device == NULL) {
        return (serial_init_result = 1);
    }

    /* Initialise the device state with resources, device instance, tx and rx
     * stream interfaces. */
    serial_device.resources = resources;
    serial_device.di = setup_device_instance(&SERIAL_DRIVER, device);
    if (device_setup(serial_device.di, serial_device.resources)
            != DEVICE_SUCCESS) {
        return (serial_init_result = 1);
    }

    serial_device.tx = (struct stream_interface *)
                            device_get_interface(serial_device.di, 0);
    serial_device.rx = (struct stream_interface *)
                            device_get_interface(serial_device.di, 1);
    if (serial_device.tx == NULL || serial_device.rx == NULL) {
        return (serial_init_result = 1);
    }
    if (device_enable(serial_device.di) != DEVICE_ENABLED) {
        return (serial_init_result = 1);
    }

    /* Create a free list of packets that we can send to the device */
    serial_device.freelist = stream_packet;
    for (i = 0; i < NUM_PACKETS - 1; i++) {
        serial_device.freelist[i].next = &serial_device.freelist[i+1];
    }
    serial_device.freelist[i].next = NULL;


    return (serial_init_result = 0);
}

