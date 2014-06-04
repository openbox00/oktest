
/* @LICENCE("Open Kernel Labs, Inc.", "2014")@ */


#include <assert.h>
#include <driver/driver.h>
#include <stdio.h>
#include <inttypes.h>
#include <util/format_bin.h>

#include <driver/mem_space.h>
#include <driver/stream.h>
#include <driver/device.h>
enum pl011_uart_v2_state {
  STATE_ENABLED,
  STATE_DISABLED,
};
struct pl011_uart_v2 {
    struct device_interface device;
    struct stream_interface rx;
    struct stream_interface tx;
    enum pl011_uart_v2_state state;
    struct resource main;
    int writec;
    int txen;
    int rxen;
    int readc;
    int writev;
    int readv;
    int fifodepth;
};

SETUP_STREAM_PROTOS(pl011_uart_v2)
SETUP_STREAM_OPS()
SETUP_DEVICE_PROTOS(pl011_uart_v2)
SETUP_DEVICE_OPS()

struct driver pl011_uart_v2 = {
   "pl011_uart_v2",
   sizeof(struct pl011_uart_v2),
   &device_ops
};

#define UARTDR_DATA(value) (uint32_t) ((value & 0xffU) << 0)
#define UARTDR_FE (uint32_t) (1 << 8)
#define UARTDR_PE (uint32_t) (1 << 9)
#define UARTDR_BE (uint32_t) (1 << 10)
#define UARTDR_OE (uint32_t) (1 << 11)

static inline uint32_t
uartdr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x0);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartdr\n", value);
#endif
    return value;
}

#define uartdr_read() uartdr_read_(device->main.resource.mem)


static inline int
uartdr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartdr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartdr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartdr_get_bit(value) uartdr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartdr_get_data_(mem_space_t space) {
    uint32_t tmp = uartdr_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdr:data -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdr_get_data() uartdr_get_data_(device->main.resource.mem)


static inline uint32_t
uartdr_get_fe_(mem_space_t space) {
    uint32_t tmp = uartdr_read_(space);
    tmp = (tmp >> 8) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdr:fe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdr_get_fe() uartdr_get_fe_(device->main.resource.mem)


static inline uint32_t
uartdr_get_pe_(mem_space_t space) {
    uint32_t tmp = uartdr_read_(space);
    tmp = (tmp >> 9) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdr:pe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdr_get_pe() uartdr_get_pe_(device->main.resource.mem)


static inline uint32_t
uartdr_get_be_(mem_space_t space) {
    uint32_t tmp = uartdr_read_(space);
    tmp = (tmp >> 10) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdr:be -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdr_get_be() uartdr_get_be_(device->main.resource.mem)


static inline uint32_t
uartdr_get_oe_(mem_space_t space) {
    uint32_t tmp = uartdr_read_(space);
    tmp = (tmp >> 11) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdr:oe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdr_get_oe() uartdr_get_oe_(device->main.resource.mem)

static inline void
uartdr_print_(mem_space_t space) {
    uint32_t tmp = uartdr_read_(space);
    printf("uartdr                 : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	data           : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
    printf("	fe             : %" PRId32 "\n", ((tmp >> 8) & 0x1U));
    printf("	pe             : %" PRId32 "\n", ((tmp >> 9) & 0x1U));
    printf("	be             : %" PRId32 "\n", ((tmp >> 10) & 0x1U));
    printf("	oe             : %" PRId32 "\n", ((tmp >> 11) & 0x1U));
}

#define uartdr_print() uartdr_print_(device->main)

static inline void
uartdr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartdr\n", value);
#endif
    mem_space_write_32(space, 0x0, value);
}

#define uartdr_write(value) uartdr_write_(device->main.resource.mem, value)


static inline void
uartdr_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartdr:%d\
", bit);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp |= (1 << bit);
    uartdr_write_(space, tmp);
}

#define uartdr_set_bit(value) uartdr_set_bit_(device->main.resource.mem, value)


static inline void
uartdr_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartdr:%d\
", bit);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp &= ~(1 << bit);
    uartdr_write_(space, tmp);
}

#define uartdr_clear_bit(value) uartdr_clear_bit_(device->main.resource.mem, value)


static inline void
uartdr_set_data_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdr:data to %x\n", value);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp &= ~ (0xffU << 0);
    tmp |= ((value & 0xffU) << 0);
    uartdr_write_(space, tmp);
}

#define uartdr_set_data(value) uartdr_set_data_(device->main.resource.mem, value)

static inline void
uartdr_set_fe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdr:fe to %x\n", value);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp &= ~ (0x1U << 8);
    tmp |= ((value & 0x1U) << 8);
    uartdr_write_(space, tmp);
}

#define uartdr_set_fe(value) uartdr_set_fe_(device->main.resource.mem, value)

static inline void
uartdr_set_pe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdr:pe to %x\n", value);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp &= ~ (0x1U << 9);
    tmp |= ((value & 0x1U) << 9);
    uartdr_write_(space, tmp);
}

#define uartdr_set_pe(value) uartdr_set_pe_(device->main.resource.mem, value)

static inline void
uartdr_set_be_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdr:be to %x\n", value);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp &= ~ (0x1U << 10);
    tmp |= ((value & 0x1U) << 10);
    uartdr_write_(space, tmp);
}

#define uartdr_set_be(value) uartdr_set_be_(device->main.resource.mem, value)

static inline void
uartdr_set_oe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdr:oe to %x\n", value);
#endif
    uint32_t tmp = uartdr_read_(space);
    tmp &= ~ (0x1U << 11);
    tmp |= ((value & 0x1U) << 11);
    uartdr_write_(space, tmp);
}

#define uartdr_set_oe(value) uartdr_set_oe_(device->main.resource.mem, value)
#define UARTRSR_FE (uint32_t) (1 << 0)
#define UARTRSR_PE (uint32_t) (1 << 1)
#define UARTRSR_BE (uint32_t) (1 << 2)
#define UARTRSR_OE (uint32_t) (1 << 3)

static inline uint32_t
uartrsr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x4);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartrsr\n", value);
#endif
    return value;
}

#define uartrsr_read() uartrsr_read_(device->main.resource.mem)


static inline int
uartrsr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartrsr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartrsr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartrsr_get_bit(value) uartrsr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartrsr_get_fe_(mem_space_t space) {
    uint32_t tmp = uartrsr_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartrsr:fe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartrsr_get_fe() uartrsr_get_fe_(device->main.resource.mem)


static inline uint32_t
uartrsr_get_pe_(mem_space_t space) {
    uint32_t tmp = uartrsr_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartrsr:pe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartrsr_get_pe() uartrsr_get_pe_(device->main.resource.mem)


static inline uint32_t
uartrsr_get_be_(mem_space_t space) {
    uint32_t tmp = uartrsr_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartrsr:be -- %x\n", tmp);
#endif
    return tmp;
}

#define uartrsr_get_be() uartrsr_get_be_(device->main.resource.mem)


static inline uint32_t
uartrsr_get_oe_(mem_space_t space) {
    uint32_t tmp = uartrsr_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartrsr:oe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartrsr_get_oe() uartrsr_get_oe_(device->main.resource.mem)

static inline void
uartrsr_print_(mem_space_t space) {
    uint32_t tmp = uartrsr_read_(space);
    printf("uartrsr                : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	fe             : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	pe             : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	be             : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	oe             : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
}

#define uartrsr_print() uartrsr_print_(device->main)

static inline void
uartrsr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartrsr\n", value);
#endif
    mem_space_write_32(space, 0x4, value);
}

#define uartrsr_write(value) uartrsr_write_(device->main.resource.mem, value)


static inline void
uartrsr_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartrsr:%d\
", bit);
#endif
    uint32_t tmp = uartrsr_read_(space);
    tmp |= (1 << bit);
    uartrsr_write_(space, tmp);
}

#define uartrsr_set_bit(value) uartrsr_set_bit_(device->main.resource.mem, value)


static inline void
uartrsr_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartrsr:%d\
", bit);
#endif
    uint32_t tmp = uartrsr_read_(space);
    tmp &= ~(1 << bit);
    uartrsr_write_(space, tmp);
}

#define uartrsr_clear_bit(value) uartrsr_clear_bit_(device->main.resource.mem, value)


static inline void
uartrsr_set_fe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartrsr:fe to %x\n", value);
#endif
    uint32_t tmp = uartrsr_read_(space);
    tmp &= ~ (0x1U << 0);
    tmp |= ((value & 0x1U) << 0);
    uartrsr_write_(space, tmp);
}

#define uartrsr_set_fe(value) uartrsr_set_fe_(device->main.resource.mem, value)

static inline void
uartrsr_set_pe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartrsr:pe to %x\n", value);
#endif
    uint32_t tmp = uartrsr_read_(space);
    tmp &= ~ (0x1U << 1);
    tmp |= ((value & 0x1U) << 1);
    uartrsr_write_(space, tmp);
}

#define uartrsr_set_pe(value) uartrsr_set_pe_(device->main.resource.mem, value)

static inline void
uartrsr_set_be_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartrsr:be to %x\n", value);
#endif
    uint32_t tmp = uartrsr_read_(space);
    tmp &= ~ (0x1U << 2);
    tmp |= ((value & 0x1U) << 2);
    uartrsr_write_(space, tmp);
}

#define uartrsr_set_be(value) uartrsr_set_be_(device->main.resource.mem, value)

static inline void
uartrsr_set_oe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartrsr:oe to %x\n", value);
#endif
    uint32_t tmp = uartrsr_read_(space);
    tmp &= ~ (0x1U << 3);
    tmp |= ((value & 0x1U) << 3);
    uartrsr_write_(space, tmp);
}

#define uartrsr_set_oe(value) uartrsr_set_oe_(device->main.resource.mem, value)
#define UARTECR_FE (uint32_t) (1 << 0)
#define UARTECR_PE (uint32_t) (1 << 1)
#define UARTECR_BE (uint32_t) (1 << 2)
#define UARTECR_OE (uint32_t) (1 << 3)

static inline uint32_t
uartecr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x4);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartecr\n", value);
#endif
    return value;
}

#define uartecr_read() uartecr_read_(device->main.resource.mem)


static inline int
uartecr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartecr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartecr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartecr_get_bit(value) uartecr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartecr_get_fe_(mem_space_t space) {
    uint32_t tmp = uartecr_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartecr:fe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartecr_get_fe() uartecr_get_fe_(device->main.resource.mem)


static inline uint32_t
uartecr_get_pe_(mem_space_t space) {
    uint32_t tmp = uartecr_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartecr:pe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartecr_get_pe() uartecr_get_pe_(device->main.resource.mem)


static inline uint32_t
uartecr_get_be_(mem_space_t space) {
    uint32_t tmp = uartecr_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartecr:be -- %x\n", tmp);
#endif
    return tmp;
}

#define uartecr_get_be() uartecr_get_be_(device->main.resource.mem)


static inline uint32_t
uartecr_get_oe_(mem_space_t space) {
    uint32_t tmp = uartecr_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartecr:oe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartecr_get_oe() uartecr_get_oe_(device->main.resource.mem)

static inline void
uartecr_print_(mem_space_t space) {
    uint32_t tmp = uartecr_read_(space);
    printf("uartecr                : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	fe             : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	pe             : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	be             : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	oe             : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
}

#define uartecr_print() uartecr_print_(device->main)

static inline void
uartecr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartecr\n", value);
#endif
    mem_space_write_32(space, 0x4, value);
}

#define uartecr_write(value) uartecr_write_(device->main.resource.mem, value)


static inline void
uartecr_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartecr:%d\
", bit);
#endif
    uint32_t tmp = uartecr_read_(space);
    tmp |= (1 << bit);
    uartecr_write_(space, tmp);
}

#define uartecr_set_bit(value) uartecr_set_bit_(device->main.resource.mem, value)


static inline void
uartecr_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartecr:%d\
", bit);
#endif
    uint32_t tmp = uartecr_read_(space);
    tmp &= ~(1 << bit);
    uartecr_write_(space, tmp);
}

#define uartecr_clear_bit(value) uartecr_clear_bit_(device->main.resource.mem, value)


static inline void
uartecr_set_fe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartecr:fe to %x\n", value);
#endif
    uint32_t tmp = uartecr_read_(space);
    tmp &= ~ (0x1U << 0);
    tmp |= ((value & 0x1U) << 0);
    uartecr_write_(space, tmp);
}

#define uartecr_set_fe(value) uartecr_set_fe_(device->main.resource.mem, value)

static inline void
uartecr_set_pe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartecr:pe to %x\n", value);
#endif
    uint32_t tmp = uartecr_read_(space);
    tmp &= ~ (0x1U << 1);
    tmp |= ((value & 0x1U) << 1);
    uartecr_write_(space, tmp);
}

#define uartecr_set_pe(value) uartecr_set_pe_(device->main.resource.mem, value)

static inline void
uartecr_set_be_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartecr:be to %x\n", value);
#endif
    uint32_t tmp = uartecr_read_(space);
    tmp &= ~ (0x1U << 2);
    tmp |= ((value & 0x1U) << 2);
    uartecr_write_(space, tmp);
}

#define uartecr_set_be(value) uartecr_set_be_(device->main.resource.mem, value)

static inline void
uartecr_set_oe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartecr:oe to %x\n", value);
#endif
    uint32_t tmp = uartecr_read_(space);
    tmp &= ~ (0x1U << 3);
    tmp |= ((value & 0x1U) << 3);
    uartecr_write_(space, tmp);
}

#define uartecr_set_oe(value) uartecr_set_oe_(device->main.resource.mem, value)
#define UARTFR_CTS (uint32_t) (1 << 0)
#define UARTFR_DSR (uint32_t) (1 << 1)
#define UARTFR_DCD (uint32_t) (1 << 2)
#define UARTFR_BUSY (uint32_t) (1 << 3)
#define UARTFR_RXFE (uint32_t) (1 << 4)
#define UARTFR_TXFF (uint32_t) (1 << 5)
#define UARTFR_RXFF (uint32_t) (1 << 6)
#define UARTFR_TXFE (uint32_t) (1 << 7)
#define UARTFR_RI (uint32_t) (1 << 8)

static inline uint32_t
uartfr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x18);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartfr\n", value);
#endif
    return value;
}

#define uartfr_read() uartfr_read_(device->main.resource.mem)


static inline int
uartfr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartfr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartfr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartfr_get_bit(value) uartfr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartfr_get_cts_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:cts -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_cts() uartfr_get_cts_(device->main.resource.mem)


static inline uint32_t
uartfr_get_dsr_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:dsr -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_dsr() uartfr_get_dsr_(device->main.resource.mem)


static inline uint32_t
uartfr_get_dcd_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:dcd -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_dcd() uartfr_get_dcd_(device->main.resource.mem)


static inline uint32_t
uartfr_get_busy_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:busy -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_busy() uartfr_get_busy_(device->main.resource.mem)


static inline uint32_t
uartfr_get_rxfe_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 4) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:rxfe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_rxfe() uartfr_get_rxfe_(device->main.resource.mem)


static inline uint32_t
uartfr_get_txff_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 5) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:txff -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_txff() uartfr_get_txff_(device->main.resource.mem)


static inline uint32_t
uartfr_get_rxff_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 6) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:rxff -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_rxff() uartfr_get_rxff_(device->main.resource.mem)


static inline uint32_t
uartfr_get_txfe_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 7) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:txfe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_txfe() uartfr_get_txfe_(device->main.resource.mem)


static inline uint32_t
uartfr_get_ri_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    tmp = (tmp >> 8) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfr:ri -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfr_get_ri() uartfr_get_ri_(device->main.resource.mem)

static inline void
uartfr_print_(mem_space_t space) {
    uint32_t tmp = uartfr_read_(space);
    printf("uartfr                 : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	cts            : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	dsr            : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	dcd            : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	busy           : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
    printf("	rxfe           : %" PRId32 "\n", ((tmp >> 4) & 0x1U));
    printf("	txff           : %" PRId32 "\n", ((tmp >> 5) & 0x1U));
    printf("	rxff           : %" PRId32 "\n", ((tmp >> 6) & 0x1U));
    printf("	txfe           : %" PRId32 "\n", ((tmp >> 7) & 0x1U));
    printf("	ri             : %" PRId32 "\n", ((tmp >> 8) & 0x1U));
}

#define uartfr_print() uartfr_print_(device->main)
#define UARTILPR_ILPDVSR(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uartilpr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x20);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartilpr\n", value);
#endif
    return value;
}

#define uartilpr_read() uartilpr_read_(device->main.resource.mem)


static inline int
uartilpr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartilpr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartilpr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartilpr_get_bit(value) uartilpr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartilpr_get_ilpdvsr_(mem_space_t space) {
    uint32_t tmp = uartilpr_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartilpr:ilpdvsr -- %x\n", tmp);
#endif
    return tmp;
}

#define uartilpr_get_ilpdvsr() uartilpr_get_ilpdvsr_(device->main.resource.mem)

static inline void
uartilpr_print_(mem_space_t space) {
    uint32_t tmp = uartilpr_read_(space);
    printf("uartilpr               : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	ilpdvsr        : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uartilpr_print() uartilpr_print_(device->main)

static inline void
uartilpr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartilpr\n", value);
#endif
    mem_space_write_32(space, 0x20, value);
}

#define uartilpr_write(value) uartilpr_write_(device->main.resource.mem, value)


static inline void
uartilpr_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartilpr:%d\
", bit);
#endif
    uint32_t tmp = uartilpr_read_(space);
    tmp |= (1 << bit);
    uartilpr_write_(space, tmp);
}

#define uartilpr_set_bit(value) uartilpr_set_bit_(device->main.resource.mem, value)


static inline void
uartilpr_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartilpr:%d\
", bit);
#endif
    uint32_t tmp = uartilpr_read_(space);
    tmp &= ~(1 << bit);
    uartilpr_write_(space, tmp);
}

#define uartilpr_clear_bit(value) uartilpr_clear_bit_(device->main.resource.mem, value)


static inline void
uartilpr_set_ilpdvsr_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartilpr:ilpdvsr to %x\n", value);
#endif
    uint32_t tmp = uartilpr_read_(space);
    tmp &= ~ (0xffU << 0);
    tmp |= ((value & 0xffU) << 0);
    uartilpr_write_(space, tmp);
}

#define uartilpr_set_ilpdvsr(value) uartilpr_set_ilpdvsr_(device->main.resource.mem, value)
#define UARTIBRD_BAUD_DIVINT(value) (uint32_t) ((value & 0xffffU) << 0)

static inline uint32_t
uartibrd_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x24);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartibrd\n", value);
#endif
    return value;
}

#define uartibrd_read() uartibrd_read_(device->main.resource.mem)


static inline int
uartibrd_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartibrd_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartibrd:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartibrd_get_bit(value) uartibrd_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartibrd_get_baud_divint_(mem_space_t space) {
    uint32_t tmp = uartibrd_read_(space);
    tmp = (tmp >> 0) & 0xffffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartibrd:baud_divint -- %x\n", tmp);
#endif
    return tmp;
}

#define uartibrd_get_baud_divint() uartibrd_get_baud_divint_(device->main.resource.mem)

static inline void
uartibrd_print_(mem_space_t space) {
    uint32_t tmp = uartibrd_read_(space);
    printf("uartibrd               : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	baud_divint    : %" PRId32 "\n", ((tmp >> 0) & 0xffffU));
}

#define uartibrd_print() uartibrd_print_(device->main)

static inline void
uartibrd_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartibrd\n", value);
#endif
    mem_space_write_32(space, 0x24, value);
}

#define uartibrd_write(value) uartibrd_write_(device->main.resource.mem, value)


static inline void
uartibrd_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartibrd:%d\
", bit);
#endif
    uint32_t tmp = uartibrd_read_(space);
    tmp |= (1 << bit);
    uartibrd_write_(space, tmp);
}

#define uartibrd_set_bit(value) uartibrd_set_bit_(device->main.resource.mem, value)


static inline void
uartibrd_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartibrd:%d\
", bit);
#endif
    uint32_t tmp = uartibrd_read_(space);
    tmp &= ~(1 << bit);
    uartibrd_write_(space, tmp);
}

#define uartibrd_clear_bit(value) uartibrd_clear_bit_(device->main.resource.mem, value)


static inline void
uartibrd_set_baud_divint_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartibrd:baud_divint to %x\n", value);
#endif
    uint32_t tmp = uartibrd_read_(space);
    tmp &= ~ (0xffffU << 0);
    tmp |= ((value & 0xffffU) << 0);
    uartibrd_write_(space, tmp);
}

#define uartibrd_set_baud_divint(value) uartibrd_set_baud_divint_(device->main.resource.mem, value)
#define UARTFBRD_BAUD_DIVFRAC(value) (uint32_t) ((value & 0x3fU) << 0)

static inline uint32_t
uartfbrd_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x28);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartfbrd\n", value);
#endif
    return value;
}

#define uartfbrd_read() uartfbrd_read_(device->main.resource.mem)


static inline int
uartfbrd_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartfbrd_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartfbrd:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartfbrd_get_bit(value) uartfbrd_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartfbrd_get_baud_divfrac_(mem_space_t space) {
    uint32_t tmp = uartfbrd_read_(space);
    tmp = (tmp >> 0) & 0x3fU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartfbrd:baud_divfrac -- %x\n", tmp);
#endif
    return tmp;
}

#define uartfbrd_get_baud_divfrac() uartfbrd_get_baud_divfrac_(device->main.resource.mem)

static inline void
uartfbrd_print_(mem_space_t space) {
    uint32_t tmp = uartfbrd_read_(space);
    printf("uartfbrd               : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	baud_divfrac   : %" PRId32 "\n", ((tmp >> 0) & 0x3fU));
}

#define uartfbrd_print() uartfbrd_print_(device->main)

static inline void
uartfbrd_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartfbrd\n", value);
#endif
    mem_space_write_32(space, 0x28, value);
}

#define uartfbrd_write(value) uartfbrd_write_(device->main.resource.mem, value)


static inline void
uartfbrd_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartfbrd:%d\
", bit);
#endif
    uint32_t tmp = uartfbrd_read_(space);
    tmp |= (1 << bit);
    uartfbrd_write_(space, tmp);
}

#define uartfbrd_set_bit(value) uartfbrd_set_bit_(device->main.resource.mem, value)


static inline void
uartfbrd_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartfbrd:%d\
", bit);
#endif
    uint32_t tmp = uartfbrd_read_(space);
    tmp &= ~(1 << bit);
    uartfbrd_write_(space, tmp);
}

#define uartfbrd_clear_bit(value) uartfbrd_clear_bit_(device->main.resource.mem, value)


static inline void
uartfbrd_set_baud_divfrac_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartfbrd:baud_divfrac to %x\n", value);
#endif
    uint32_t tmp = uartfbrd_read_(space);
    tmp &= ~ (0x3fU << 0);
    tmp |= ((value & 0x3fU) << 0);
    uartfbrd_write_(space, tmp);
}

#define uartfbrd_set_baud_divfrac(value) uartfbrd_set_baud_divfrac_(device->main.resource.mem, value)
#define UARTLCR_H_BRK (uint32_t) (1 << 0)
#define UARTLCR_H_PEN (uint32_t) (1 << 1)
#define UARTLCR_H_EPS (uint32_t) (1 << 2)
#define UARTLCR_H_STP2 (uint32_t) (1 << 3)
#define UARTLCR_H_FEN (uint32_t) (1 << 4)
#define UARTLCR_H_WLEN(value) (uint32_t) ((value & 0x3U) << 5)
#define UARTLCR_H_SPS (uint32_t) (1 << 7)

static inline uint32_t
uartlcr_h_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x2c);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartlcr_h\n", value);
#endif
    return value;
}

#define uartlcr_h_read() uartlcr_h_read_(device->main.resource.mem)


static inline int
uartlcr_h_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartlcr_h_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartlcr_h:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_bit(value) uartlcr_h_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartlcr_h_get_brk_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:brk -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_brk() uartlcr_h_get_brk_(device->main.resource.mem)


static inline uint32_t
uartlcr_h_get_pen_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:pen -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_pen() uartlcr_h_get_pen_(device->main.resource.mem)


static inline uint32_t
uartlcr_h_get_eps_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:eps -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_eps() uartlcr_h_get_eps_(device->main.resource.mem)


static inline uint32_t
uartlcr_h_get_stp2_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:stp2 -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_stp2() uartlcr_h_get_stp2_(device->main.resource.mem)


static inline uint32_t
uartlcr_h_get_fen_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 4) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:fen -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_fen() uartlcr_h_get_fen_(device->main.resource.mem)


static inline uint32_t
uartlcr_h_get_wlen_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 5) & 0x3U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:wlen -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_wlen() uartlcr_h_get_wlen_(device->main.resource.mem)


static inline uint32_t
uartlcr_h_get_sps_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    tmp = (tmp >> 7) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartlcr_h:sps -- %x\n", tmp);
#endif
    return tmp;
}

#define uartlcr_h_get_sps() uartlcr_h_get_sps_(device->main.resource.mem)

static inline void
uartlcr_h_print_(mem_space_t space) {
    uint32_t tmp = uartlcr_h_read_(space);
    printf("uartlcr_h              : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	brk            : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	pen            : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	eps            : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	stp2           : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
    printf("	fen            : %" PRId32 "\n", ((tmp >> 4) & 0x1U));
    printf("	wlen           : %" PRId32 "\n", ((tmp >> 5) & 0x3U));
    printf("	sps            : %" PRId32 "\n", ((tmp >> 7) & 0x1U));
}

#define uartlcr_h_print() uartlcr_h_print_(device->main)

static inline void
uartlcr_h_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartlcr_h\n", value);
#endif
    mem_space_write_32(space, 0x2c, value);
}

#define uartlcr_h_write(value) uartlcr_h_write_(device->main.resource.mem, value)


static inline void
uartlcr_h_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartlcr_h:%d\
", bit);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp |= (1 << bit);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_bit(value) uartlcr_h_set_bit_(device->main.resource.mem, value)


static inline void
uartlcr_h_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartlcr_h:%d\
", bit);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~(1 << bit);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_clear_bit(value) uartlcr_h_clear_bit_(device->main.resource.mem, value)


static inline void
uartlcr_h_set_brk_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:brk to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x1U << 0);
    tmp |= ((value & 0x1U) << 0);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_brk(value) uartlcr_h_set_brk_(device->main.resource.mem, value)

static inline void
uartlcr_h_set_pen_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:pen to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x1U << 1);
    tmp |= ((value & 0x1U) << 1);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_pen(value) uartlcr_h_set_pen_(device->main.resource.mem, value)

static inline void
uartlcr_h_set_eps_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:eps to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x1U << 2);
    tmp |= ((value & 0x1U) << 2);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_eps(value) uartlcr_h_set_eps_(device->main.resource.mem, value)

static inline void
uartlcr_h_set_stp2_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:stp2 to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x1U << 3);
    tmp |= ((value & 0x1U) << 3);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_stp2(value) uartlcr_h_set_stp2_(device->main.resource.mem, value)

static inline void
uartlcr_h_set_fen_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:fen to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x1U << 4);
    tmp |= ((value & 0x1U) << 4);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_fen(value) uartlcr_h_set_fen_(device->main.resource.mem, value)

static inline void
uartlcr_h_set_wlen_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:wlen to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x3U << 5);
    tmp |= ((value & 0x3U) << 5);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_wlen(value) uartlcr_h_set_wlen_(device->main.resource.mem, value)

static inline void
uartlcr_h_set_sps_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartlcr_h:sps to %x\n", value);
#endif
    uint32_t tmp = uartlcr_h_read_(space);
    tmp &= ~ (0x1U << 7);
    tmp |= ((value & 0x1U) << 7);
    uartlcr_h_write_(space, tmp);
}

#define uartlcr_h_set_sps(value) uartlcr_h_set_sps_(device->main.resource.mem, value)
#define UARTCR_UARTEN (uint32_t) (1 << 0)
#define UARTCR_SIREN (uint32_t) (1 << 1)
#define UARTCR_SIRLP (uint32_t) (1 << 2)
#define UARTCR_LBE (uint32_t) (1 << 7)
#define UARTCR_TXE (uint32_t) (1 << 8)
#define UARTCR_RXE (uint32_t) (1 << 9)
#define UARTCR_DTR (uint32_t) (1 << 10)
#define UARTCR_RTS (uint32_t) (1 << 11)
#define UARTCR_OUT1 (uint32_t) (1 << 12)
#define UARTCR_OUT2 (uint32_t) (1 << 13)
#define UARTCR_RTSEN (uint32_t) (1 << 14)
#define UARTCR_CTSEN (uint32_t) (1 << 15)

static inline uint32_t
uartcr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x30);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartcr\n", value);
#endif
    return value;
}

#define uartcr_read() uartcr_read_(device->main.resource.mem)


static inline int
uartcr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartcr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartcr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartcr_get_bit(value) uartcr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartcr_get_uarten_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:uarten -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_uarten() uartcr_get_uarten_(device->main.resource.mem)


static inline uint32_t
uartcr_get_siren_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:siren -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_siren() uartcr_get_siren_(device->main.resource.mem)


static inline uint32_t
uartcr_get_sirlp_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:sirlp -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_sirlp() uartcr_get_sirlp_(device->main.resource.mem)


static inline uint32_t
uartcr_get_lbe_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 7) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:lbe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_lbe() uartcr_get_lbe_(device->main.resource.mem)


static inline uint32_t
uartcr_get_txe_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 8) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:txe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_txe() uartcr_get_txe_(device->main.resource.mem)


static inline uint32_t
uartcr_get_rxe_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 9) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:rxe -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_rxe() uartcr_get_rxe_(device->main.resource.mem)


static inline uint32_t
uartcr_get_dtr_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 10) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:dtr -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_dtr() uartcr_get_dtr_(device->main.resource.mem)


static inline uint32_t
uartcr_get_rts_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 11) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:rts -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_rts() uartcr_get_rts_(device->main.resource.mem)


static inline uint32_t
uartcr_get_out1_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 12) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:out1 -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_out1() uartcr_get_out1_(device->main.resource.mem)


static inline uint32_t
uartcr_get_out2_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 13) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:out2 -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_out2() uartcr_get_out2_(device->main.resource.mem)


static inline uint32_t
uartcr_get_rtsen_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 14) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:rtsen -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_rtsen() uartcr_get_rtsen_(device->main.resource.mem)


static inline uint32_t
uartcr_get_ctsen_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    tmp = (tmp >> 15) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartcr:ctsen -- %x\n", tmp);
#endif
    return tmp;
}

#define uartcr_get_ctsen() uartcr_get_ctsen_(device->main.resource.mem)

static inline void
uartcr_print_(mem_space_t space) {
    uint32_t tmp = uartcr_read_(space);
    printf("uartcr                 : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	uarten         : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	siren          : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	sirlp          : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	lbe            : %" PRId32 "\n", ((tmp >> 7) & 0x1U));
    printf("	txe            : %" PRId32 "\n", ((tmp >> 8) & 0x1U));
    printf("	rxe            : %" PRId32 "\n", ((tmp >> 9) & 0x1U));
    printf("	dtr            : %" PRId32 "\n", ((tmp >> 10) & 0x1U));
    printf("	rts            : %" PRId32 "\n", ((tmp >> 11) & 0x1U));
    printf("	out1           : %" PRId32 "\n", ((tmp >> 12) & 0x1U));
    printf("	out2           : %" PRId32 "\n", ((tmp >> 13) & 0x1U));
    printf("	rtsen          : %" PRId32 "\n", ((tmp >> 14) & 0x1U));
    printf("	ctsen          : %" PRId32 "\n", ((tmp >> 15) & 0x1U));
}

#define uartcr_print() uartcr_print_(device->main)

static inline void
uartcr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartcr\n", value);
#endif
    mem_space_write_32(space, 0x30, value);
}

#define uartcr_write(value) uartcr_write_(device->main.resource.mem, value)


static inline void
uartcr_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartcr:%d\
", bit);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp |= (1 << bit);
    uartcr_write_(space, tmp);
}

#define uartcr_set_bit(value) uartcr_set_bit_(device->main.resource.mem, value)


static inline void
uartcr_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartcr:%d\
", bit);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~(1 << bit);
    uartcr_write_(space, tmp);
}

#define uartcr_clear_bit(value) uartcr_clear_bit_(device->main.resource.mem, value)


static inline void
uartcr_set_uarten_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:uarten to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 0);
    tmp |= ((value & 0x1U) << 0);
    uartcr_write_(space, tmp);
}

#define uartcr_set_uarten(value) uartcr_set_uarten_(device->main.resource.mem, value)

static inline void
uartcr_set_siren_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:siren to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 1);
    tmp |= ((value & 0x1U) << 1);
    uartcr_write_(space, tmp);
}

#define uartcr_set_siren(value) uartcr_set_siren_(device->main.resource.mem, value)

static inline void
uartcr_set_sirlp_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:sirlp to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 2);
    tmp |= ((value & 0x1U) << 2);
    uartcr_write_(space, tmp);
}

#define uartcr_set_sirlp(value) uartcr_set_sirlp_(device->main.resource.mem, value)

static inline void
uartcr_set_lbe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:lbe to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 7);
    tmp |= ((value & 0x1U) << 7);
    uartcr_write_(space, tmp);
}

#define uartcr_set_lbe(value) uartcr_set_lbe_(device->main.resource.mem, value)

static inline void
uartcr_set_txe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:txe to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 8);
    tmp |= ((value & 0x1U) << 8);
    uartcr_write_(space, tmp);
}

#define uartcr_set_txe(value) uartcr_set_txe_(device->main.resource.mem, value)

static inline void
uartcr_set_rxe_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:rxe to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 9);
    tmp |= ((value & 0x1U) << 9);
    uartcr_write_(space, tmp);
}

#define uartcr_set_rxe(value) uartcr_set_rxe_(device->main.resource.mem, value)

static inline void
uartcr_set_dtr_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:dtr to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 10);
    tmp |= ((value & 0x1U) << 10);
    uartcr_write_(space, tmp);
}

#define uartcr_set_dtr(value) uartcr_set_dtr_(device->main.resource.mem, value)

static inline void
uartcr_set_rts_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:rts to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 11);
    tmp |= ((value & 0x1U) << 11);
    uartcr_write_(space, tmp);
}

#define uartcr_set_rts(value) uartcr_set_rts_(device->main.resource.mem, value)

static inline void
uartcr_set_out1_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:out1 to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 12);
    tmp |= ((value & 0x1U) << 12);
    uartcr_write_(space, tmp);
}

#define uartcr_set_out1(value) uartcr_set_out1_(device->main.resource.mem, value)

static inline void
uartcr_set_out2_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:out2 to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 13);
    tmp |= ((value & 0x1U) << 13);
    uartcr_write_(space, tmp);
}

#define uartcr_set_out2(value) uartcr_set_out2_(device->main.resource.mem, value)

static inline void
uartcr_set_rtsen_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:rtsen to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 14);
    tmp |= ((value & 0x1U) << 14);
    uartcr_write_(space, tmp);
}

#define uartcr_set_rtsen(value) uartcr_set_rtsen_(device->main.resource.mem, value)

static inline void
uartcr_set_ctsen_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartcr:ctsen to %x\n", value);
#endif
    uint32_t tmp = uartcr_read_(space);
    tmp &= ~ (0x1U << 15);
    tmp |= ((value & 0x1U) << 15);
    uartcr_write_(space, tmp);
}

#define uartcr_set_ctsen(value) uartcr_set_ctsen_(device->main.resource.mem, value)
#define UARTIFLS_TXIFLSEL(value) (uint32_t) ((value & 0x7U) << 0)
#define UARTIFLS_RXIFLSEL(value) (uint32_t) ((value & 0x7U) << 3)

static inline uint32_t
uartifls_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x34);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartifls\n", value);
#endif
    return value;
}

#define uartifls_read() uartifls_read_(device->main.resource.mem)


static inline int
uartifls_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartifls_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartifls:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartifls_get_bit(value) uartifls_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartifls_get_txiflsel_(mem_space_t space) {
    uint32_t tmp = uartifls_read_(space);
    tmp = (tmp >> 0) & 0x7U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartifls:txiflsel -- %x\n", tmp);
#endif
    return tmp;
}

#define uartifls_get_txiflsel() uartifls_get_txiflsel_(device->main.resource.mem)


static inline uint32_t
uartifls_get_rxiflsel_(mem_space_t space) {
    uint32_t tmp = uartifls_read_(space);
    tmp = (tmp >> 3) & 0x7U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartifls:rxiflsel -- %x\n", tmp);
#endif
    return tmp;
}

#define uartifls_get_rxiflsel() uartifls_get_rxiflsel_(device->main.resource.mem)

static inline void
uartifls_print_(mem_space_t space) {
    uint32_t tmp = uartifls_read_(space);
    printf("uartifls               : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	txiflsel       : %" PRId32 "\n", ((tmp >> 0) & 0x7U));
    printf("	rxiflsel       : %" PRId32 "\n", ((tmp >> 3) & 0x7U));
}

#define uartifls_print() uartifls_print_(device->main)

static inline void
uartifls_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartifls\n", value);
#endif
    mem_space_write_32(space, 0x34, value);
}

#define uartifls_write(value) uartifls_write_(device->main.resource.mem, value)


static inline void
uartifls_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartifls:%d\
", bit);
#endif
    uint32_t tmp = uartifls_read_(space);
    tmp |= (1 << bit);
    uartifls_write_(space, tmp);
}

#define uartifls_set_bit(value) uartifls_set_bit_(device->main.resource.mem, value)


static inline void
uartifls_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartifls:%d\
", bit);
#endif
    uint32_t tmp = uartifls_read_(space);
    tmp &= ~(1 << bit);
    uartifls_write_(space, tmp);
}

#define uartifls_clear_bit(value) uartifls_clear_bit_(device->main.resource.mem, value)


static inline void
uartifls_set_txiflsel_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartifls:txiflsel to %x\n", value);
#endif
    uint32_t tmp = uartifls_read_(space);
    tmp &= ~ (0x7U << 0);
    tmp |= ((value & 0x7U) << 0);
    uartifls_write_(space, tmp);
}

#define uartifls_set_txiflsel(value) uartifls_set_txiflsel_(device->main.resource.mem, value)

static inline void
uartifls_set_rxiflsel_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartifls:rxiflsel to %x\n", value);
#endif
    uint32_t tmp = uartifls_read_(space);
    tmp &= ~ (0x7U << 3);
    tmp |= ((value & 0x7U) << 3);
    uartifls_write_(space, tmp);
}

#define uartifls_set_rxiflsel(value) uartifls_set_rxiflsel_(device->main.resource.mem, value)
#define UARTIMSC_RIMIM (uint32_t) (1 << 0)
#define UARTIMSC_CTSMIM (uint32_t) (1 << 1)
#define UARTIMSC_DCDMIM (uint32_t) (1 << 2)
#define UARTIMSC_DSRMIM (uint32_t) (1 << 3)
#define UARTIMSC_RXIM (uint32_t) (1 << 4)
#define UARTIMSC_TXIM (uint32_t) (1 << 5)
#define UARTIMSC_RTIM (uint32_t) (1 << 6)
#define UARTIMSC_FEIM (uint32_t) (1 << 7)
#define UARTIMSC_PEIM (uint32_t) (1 << 8)
#define UARTIMSC_BEIM (uint32_t) (1 << 9)
#define UARTIMSC_OEIM (uint32_t) (1 << 10)

static inline uint32_t
uartimsc_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x38);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartimsc\n", value);
#endif
    return value;
}

#define uartimsc_read() uartimsc_read_(device->main.resource.mem)


static inline int
uartimsc_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartimsc_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartimsc:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartimsc_get_bit(value) uartimsc_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartimsc_get_rimim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:rimim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_rimim() uartimsc_get_rimim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_ctsmim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:ctsmim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_ctsmim() uartimsc_get_ctsmim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_dcdmim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:dcdmim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_dcdmim() uartimsc_get_dcdmim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_dsrmim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:dsrmim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_dsrmim() uartimsc_get_dsrmim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_rxim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 4) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:rxim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_rxim() uartimsc_get_rxim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_txim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 5) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:txim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_txim() uartimsc_get_txim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_rtim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 6) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:rtim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_rtim() uartimsc_get_rtim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_feim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 7) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:feim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_feim() uartimsc_get_feim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_peim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 8) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:peim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_peim() uartimsc_get_peim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_beim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 9) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:beim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_beim() uartimsc_get_beim_(device->main.resource.mem)


static inline uint32_t
uartimsc_get_oeim_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    tmp = (tmp >> 10) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartimsc:oeim -- %x\n", tmp);
#endif
    return tmp;
}

#define uartimsc_get_oeim() uartimsc_get_oeim_(device->main.resource.mem)

static inline void
uartimsc_print_(mem_space_t space) {
    uint32_t tmp = uartimsc_read_(space);
    printf("uartimsc               : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	rimim          : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	ctsmim         : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	dcdmim         : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	dsrmim         : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
    printf("	rxim           : %" PRId32 "\n", ((tmp >> 4) & 0x1U));
    printf("	txim           : %" PRId32 "\n", ((tmp >> 5) & 0x1U));
    printf("	rtim           : %" PRId32 "\n", ((tmp >> 6) & 0x1U));
    printf("	feim           : %" PRId32 "\n", ((tmp >> 7) & 0x1U));
    printf("	peim           : %" PRId32 "\n", ((tmp >> 8) & 0x1U));
    printf("	beim           : %" PRId32 "\n", ((tmp >> 9) & 0x1U));
    printf("	oeim           : %" PRId32 "\n", ((tmp >> 10) & 0x1U));
}

#define uartimsc_print() uartimsc_print_(device->main)

static inline void
uartimsc_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartimsc\n", value);
#endif
    mem_space_write_32(space, 0x38, value);
}

#define uartimsc_write(value) uartimsc_write_(device->main.resource.mem, value)


static inline void
uartimsc_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartimsc:%d\
", bit);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp |= (1 << bit);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_bit(value) uartimsc_set_bit_(device->main.resource.mem, value)


static inline void
uartimsc_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartimsc:%d\
", bit);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~(1 << bit);
    uartimsc_write_(space, tmp);
}

#define uartimsc_clear_bit(value) uartimsc_clear_bit_(device->main.resource.mem, value)


static inline void
uartimsc_set_rimim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:rimim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 0);
    tmp |= ((value & 0x1U) << 0);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_rimim(value) uartimsc_set_rimim_(device->main.resource.mem, value)

static inline void
uartimsc_set_ctsmim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:ctsmim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 1);
    tmp |= ((value & 0x1U) << 1);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_ctsmim(value) uartimsc_set_ctsmim_(device->main.resource.mem, value)

static inline void
uartimsc_set_dcdmim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:dcdmim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 2);
    tmp |= ((value & 0x1U) << 2);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_dcdmim(value) uartimsc_set_dcdmim_(device->main.resource.mem, value)

static inline void
uartimsc_set_dsrmim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:dsrmim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 3);
    tmp |= ((value & 0x1U) << 3);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_dsrmim(value) uartimsc_set_dsrmim_(device->main.resource.mem, value)

static inline void
uartimsc_set_rxim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:rxim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 4);
    tmp |= ((value & 0x1U) << 4);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_rxim(value) uartimsc_set_rxim_(device->main.resource.mem, value)

static inline void
uartimsc_set_txim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:txim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 5);
    tmp |= ((value & 0x1U) << 5);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_txim(value) uartimsc_set_txim_(device->main.resource.mem, value)

static inline void
uartimsc_set_rtim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:rtim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 6);
    tmp |= ((value & 0x1U) << 6);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_rtim(value) uartimsc_set_rtim_(device->main.resource.mem, value)

static inline void
uartimsc_set_feim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:feim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 7);
    tmp |= ((value & 0x1U) << 7);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_feim(value) uartimsc_set_feim_(device->main.resource.mem, value)

static inline void
uartimsc_set_peim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:peim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 8);
    tmp |= ((value & 0x1U) << 8);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_peim(value) uartimsc_set_peim_(device->main.resource.mem, value)

static inline void
uartimsc_set_beim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:beim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 9);
    tmp |= ((value & 0x1U) << 9);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_beim(value) uartimsc_set_beim_(device->main.resource.mem, value)

static inline void
uartimsc_set_oeim_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartimsc:oeim to %x\n", value);
#endif
    uint32_t tmp = uartimsc_read_(space);
    tmp &= ~ (0x1U << 10);
    tmp |= ((value & 0x1U) << 10);
    uartimsc_write_(space, tmp);
}

#define uartimsc_set_oeim(value) uartimsc_set_oeim_(device->main.resource.mem, value)
#define UARTRIS_RIRMIS (uint32_t) (1 << 0)
#define UARTRIS_CTSRMIS (uint32_t) (1 << 1)
#define UARTRIS_DCDRMIS (uint32_t) (1 << 2)
#define UARTRIS_DSRRMIS (uint32_t) (1 << 3)
#define UARTRIS_RXRIS (uint32_t) (1 << 4)
#define UARTRIS_TXRIS (uint32_t) (1 << 5)
#define UARTRIS_RTRIS (uint32_t) (1 << 6)
#define UARTRIS_FERIS (uint32_t) (1 << 7)
#define UARTRIS_PERIS (uint32_t) (1 << 8)
#define UARTRIS_BERIS (uint32_t) (1 << 9)
#define UARTRIS_OERIS (uint32_t) (1 << 10)

static inline uint32_t
uartris_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x3c);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartris\n", value);
#endif
    return value;
}

#define uartris_read() uartris_read_(device->main.resource.mem)


static inline int
uartris_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartris_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartris:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartris_get_bit(value) uartris_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartris_get_rirmis_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:rirmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_rirmis() uartris_get_rirmis_(device->main.resource.mem)


static inline uint32_t
uartris_get_ctsrmis_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:ctsrmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_ctsrmis() uartris_get_ctsrmis_(device->main.resource.mem)


static inline uint32_t
uartris_get_dcdrmis_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:dcdrmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_dcdrmis() uartris_get_dcdrmis_(device->main.resource.mem)


static inline uint32_t
uartris_get_dsrrmis_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:dsrrmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_dsrrmis() uartris_get_dsrrmis_(device->main.resource.mem)


static inline uint32_t
uartris_get_rxris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 4) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:rxris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_rxris() uartris_get_rxris_(device->main.resource.mem)


static inline uint32_t
uartris_get_txris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 5) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:txris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_txris() uartris_get_txris_(device->main.resource.mem)


static inline uint32_t
uartris_get_rtris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 6) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:rtris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_rtris() uartris_get_rtris_(device->main.resource.mem)


static inline uint32_t
uartris_get_feris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 7) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:feris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_feris() uartris_get_feris_(device->main.resource.mem)


static inline uint32_t
uartris_get_peris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 8) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:peris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_peris() uartris_get_peris_(device->main.resource.mem)


static inline uint32_t
uartris_get_beris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 9) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:beris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_beris() uartris_get_beris_(device->main.resource.mem)


static inline uint32_t
uartris_get_oeris_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    tmp = (tmp >> 10) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartris:oeris -- %x\n", tmp);
#endif
    return tmp;
}

#define uartris_get_oeris() uartris_get_oeris_(device->main.resource.mem)

static inline void
uartris_print_(mem_space_t space) {
    uint32_t tmp = uartris_read_(space);
    printf("uartris                : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	rirmis         : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	ctsrmis        : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	dcdrmis        : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	dsrrmis        : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
    printf("	rxris          : %" PRId32 "\n", ((tmp >> 4) & 0x1U));
    printf("	txris          : %" PRId32 "\n", ((tmp >> 5) & 0x1U));
    printf("	rtris          : %" PRId32 "\n", ((tmp >> 6) & 0x1U));
    printf("	feris          : %" PRId32 "\n", ((tmp >> 7) & 0x1U));
    printf("	peris          : %" PRId32 "\n", ((tmp >> 8) & 0x1U));
    printf("	beris          : %" PRId32 "\n", ((tmp >> 9) & 0x1U));
    printf("	oeris          : %" PRId32 "\n", ((tmp >> 10) & 0x1U));
}

#define uartris_print() uartris_print_(device->main)
#define UARTMIS_RIMMIS (uint32_t) (1 << 0)
#define UARTMIS_CTSMMIS (uint32_t) (1 << 1)
#define UARTMIS_DCDMMIS (uint32_t) (1 << 2)
#define UARTMIS_DSRMMIS (uint32_t) (1 << 3)
#define UARTMIS_RXMIS (uint32_t) (1 << 4)
#define UARTMIS_TXMIS (uint32_t) (1 << 5)
#define UARTMIS_RTMIS (uint32_t) (1 << 6)
#define UARTMIS_FEMIS (uint32_t) (1 << 7)
#define UARTMIS_PEMIS (uint32_t) (1 << 8)
#define UARTMIS_BEMIS (uint32_t) (1 << 9)
#define UARTMIS_OEMIS (uint32_t) (1 << 10)

static inline uint32_t
uartmis_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x40);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartmis\n", value);
#endif
    return value;
}

#define uartmis_read() uartmis_read_(device->main.resource.mem)


static inline int
uartmis_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartmis_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartmis:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartmis_get_bit(value) uartmis_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartmis_get_rimmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:rimmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_rimmis() uartmis_get_rimmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_ctsmmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:ctsmmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_ctsmmis() uartmis_get_ctsmmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_dcdmmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:dcdmmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_dcdmmis() uartmis_get_dcdmmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_dsrmmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 3) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:dsrmmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_dsrmmis() uartmis_get_dsrmmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_rxmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 4) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:rxmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_rxmis() uartmis_get_rxmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_txmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 5) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:txmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_txmis() uartmis_get_txmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_rtmis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 6) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:rtmis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_rtmis() uartmis_get_rtmis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_femis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 7) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:femis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_femis() uartmis_get_femis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_pemis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 8) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:pemis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_pemis() uartmis_get_pemis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_bemis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 9) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:bemis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_bemis() uartmis_get_bemis_(device->main.resource.mem)


static inline uint32_t
uartmis_get_oemis_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    tmp = (tmp >> 10) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartmis:oemis -- %x\n", tmp);
#endif
    return tmp;
}

#define uartmis_get_oemis() uartmis_get_oemis_(device->main.resource.mem)

static inline void
uartmis_print_(mem_space_t space) {
    uint32_t tmp = uartmis_read_(space);
    printf("uartmis                : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	rimmis         : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	ctsmmis        : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	dcdmmis        : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
    printf("	dsrmmis        : %" PRId32 "\n", ((tmp >> 3) & 0x1U));
    printf("	rxmis          : %" PRId32 "\n", ((tmp >> 4) & 0x1U));
    printf("	txmis          : %" PRId32 "\n", ((tmp >> 5) & 0x1U));
    printf("	rtmis          : %" PRId32 "\n", ((tmp >> 6) & 0x1U));
    printf("	femis          : %" PRId32 "\n", ((tmp >> 7) & 0x1U));
    printf("	pemis          : %" PRId32 "\n", ((tmp >> 8) & 0x1U));
    printf("	bemis          : %" PRId32 "\n", ((tmp >> 9) & 0x1U));
    printf("	oemis          : %" PRId32 "\n", ((tmp >> 10) & 0x1U));
}

#define uartmis_print() uartmis_print_(device->main)
#define UARTICR_RIMIC (uint32_t) (1 << 0)
#define UARTICR_CTSMIC (uint32_t) (1 << 1)
#define UARTICR_DCDMIC (uint32_t) (1 << 2)
#define UARTICR_DSRMIC (uint32_t) (1 << 3)
#define UARTICR_RXIC (uint32_t) (1 << 4)
#define UARTICR_TXIC (uint32_t) (1 << 5)
#define UARTICR_RTIC (uint32_t) (1 << 6)
#define UARTICR_FEIC (uint32_t) (1 << 7)
#define UARTICR_PEIC (uint32_t) (1 << 8)
#define UARTICR_BEIC (uint32_t) (1 << 9)
#define UARTICR_OEIC (uint32_t) (1 << 10)

static inline void
uarticr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uarticr\n", value);
#endif
    mem_space_write_32(space, 0x44, value);
}

#define uarticr_write(value) uarticr_write_(device->main.resource.mem, value)

#define UARTDMACR_RXDMAE (uint32_t) (1 << 0)
#define UARTDMACR_TXDMAE (uint32_t) (1 << 1)
#define UARTDMACR_DMAONERR (uint32_t) (1 << 2)

static inline uint32_t
uartdmacr_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0x48);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uartdmacr\n", value);
#endif
    return value;
}

#define uartdmacr_read() uartdmacr_read_(device->main.resource.mem)


static inline int
uartdmacr_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uartdmacr_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uartdmacr:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uartdmacr_get_bit(value) uartdmacr_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uartdmacr_get_rxdmae_(mem_space_t space) {
    uint32_t tmp = uartdmacr_read_(space);
    tmp = (tmp >> 0) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdmacr:rxdmae -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdmacr_get_rxdmae() uartdmacr_get_rxdmae_(device->main.resource.mem)


static inline uint32_t
uartdmacr_get_txdmae_(mem_space_t space) {
    uint32_t tmp = uartdmacr_read_(space);
    tmp = (tmp >> 1) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdmacr:txdmae -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdmacr_get_txdmae() uartdmacr_get_txdmae_(device->main.resource.mem)


static inline uint32_t
uartdmacr_get_dmaonerr_(mem_space_t space) {
    uint32_t tmp = uartdmacr_read_(space);
    tmp = (tmp >> 2) & 0x1U;
#if defined(DEBUG_DDL)
    printf("\tGetting field uartdmacr:dmaonerr -- %x\n", tmp);
#endif
    return tmp;
}

#define uartdmacr_get_dmaonerr() uartdmacr_get_dmaonerr_(device->main.resource.mem)

static inline void
uartdmacr_print_(mem_space_t space) {
    uint32_t tmp = uartdmacr_read_(space);
    printf("uartdmacr              : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	rxdmae         : %" PRId32 "\n", ((tmp >> 0) & 0x1U));
    printf("	txdmae         : %" PRId32 "\n", ((tmp >> 1) & 0x1U));
    printf("	dmaonerr       : %" PRId32 "\n", ((tmp >> 2) & 0x1U));
}

#define uartdmacr_print() uartdmacr_print_(device->main)

static inline void
uartdmacr_write_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tWriting %x to register uartdmacr\n", value);
#endif
    mem_space_write_32(space, 0x48, value);
}

#define uartdmacr_write(value) uartdmacr_write_(device->main.resource.mem, value)


static inline void
uartdmacr_set_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Setting bit uartdmacr:%d\
", bit);
#endif
    uint32_t tmp = uartdmacr_read_(space);
    tmp |= (1 << bit);
    uartdmacr_write_(space, tmp);
}

#define uartdmacr_set_bit(value) uartdmacr_set_bit_(device->main.resource.mem, value)


static inline void
uartdmacr_clear_bit_(mem_space_t space, uint8_t bit) {
#if defined(DEBUG_DDL)
    printf("Clear bit uartdmacr:%d\
", bit);
#endif
    uint32_t tmp = uartdmacr_read_(space);
    tmp &= ~(1 << bit);
    uartdmacr_write_(space, tmp);
}

#define uartdmacr_clear_bit(value) uartdmacr_clear_bit_(device->main.resource.mem, value)


static inline void
uartdmacr_set_rxdmae_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdmacr:rxdmae to %x\n", value);
#endif
    uint32_t tmp = uartdmacr_read_(space);
    tmp &= ~ (0x1U << 0);
    tmp |= ((value & 0x1U) << 0);
    uartdmacr_write_(space, tmp);
}

#define uartdmacr_set_rxdmae(value) uartdmacr_set_rxdmae_(device->main.resource.mem, value)

static inline void
uartdmacr_set_txdmae_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdmacr:txdmae to %x\n", value);
#endif
    uint32_t tmp = uartdmacr_read_(space);
    tmp &= ~ (0x1U << 1);
    tmp |= ((value & 0x1U) << 1);
    uartdmacr_write_(space, tmp);
}

#define uartdmacr_set_txdmae(value) uartdmacr_set_txdmae_(device->main.resource.mem, value)

static inline void
uartdmacr_set_dmaonerr_(mem_space_t space, uint32_t value) {
#if defined(DEBUG_DDL)
    printf("\tSetting field uartdmacr:dmaonerr to %x\n", value);
#endif
    uint32_t tmp = uartdmacr_read_(space);
    tmp &= ~ (0x1U << 2);
    tmp |= ((value & 0x1U) << 2);
    uartdmacr_write_(space, tmp);
}

#define uartdmacr_set_dmaonerr(value) uartdmacr_set_dmaonerr_(device->main.resource.mem, value)
#define UART_PERIPH_ID0_PARTNUMBER0(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uart_periph_id0_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xfe0);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_periph_id0\n", value);
#endif
    return value;
}

#define uart_periph_id0_read() uart_periph_id0_read_(device->main.resource.mem)


static inline int
uart_periph_id0_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_periph_id0_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_periph_id0:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_periph_id0_get_bit(value) uart_periph_id0_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_periph_id0_get_PartNumber0_(mem_space_t space) {
    uint32_t tmp = uart_periph_id0_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id0:PartNumber0 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id0_get_PartNumber0() uart_periph_id0_get_PartNumber0_(device->main.resource.mem)

static inline void
uart_periph_id0_print_(mem_space_t space) {
    uint32_t tmp = uart_periph_id0_read_(space);
    printf("uart_periph_id0        : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	PartNumber0    : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uart_periph_id0_print() uart_periph_id0_print_(device->main)
#define UART_PERIPH_ID1_PARTNUMBER1(value) (uint32_t) ((value & 0xfU) << 0)
#define UART_PERIPH_ID1_DESIGNER0(value) (uint32_t) ((value & 0xfU) << 4)

static inline uint32_t
uart_periph_id1_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xfe4);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_periph_id1\n", value);
#endif
    return value;
}

#define uart_periph_id1_read() uart_periph_id1_read_(device->main.resource.mem)


static inline int
uart_periph_id1_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_periph_id1_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_periph_id1:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_periph_id1_get_bit(value) uart_periph_id1_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_periph_id1_get_PartNumber1_(mem_space_t space) {
    uint32_t tmp = uart_periph_id1_read_(space);
    tmp = (tmp >> 0) & 0xfU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id1:PartNumber1 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id1_get_PartNumber1() uart_periph_id1_get_PartNumber1_(device->main.resource.mem)


static inline uint32_t
uart_periph_id1_get_Designer0_(mem_space_t space) {
    uint32_t tmp = uart_periph_id1_read_(space);
    tmp = (tmp >> 4) & 0xfU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id1:Designer0 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id1_get_Designer0() uart_periph_id1_get_Designer0_(device->main.resource.mem)

static inline void
uart_periph_id1_print_(mem_space_t space) {
    uint32_t tmp = uart_periph_id1_read_(space);
    printf("uart_periph_id1        : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	PartNumber1    : %" PRId32 "\n", ((tmp >> 0) & 0xfU));
    printf("	Designer0      : %" PRId32 "\n", ((tmp >> 4) & 0xfU));
}

#define uart_periph_id1_print() uart_periph_id1_print_(device->main)
#define UART_PERIPH_ID2_DESIGNER1(value) (uint32_t) ((value & 0xfU) << 0)
#define UART_PERIPH_ID2_REVISION(value) (uint32_t) ((value & 0xfU) << 4)

static inline uint32_t
uart_periph_id2_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xfe8);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_periph_id2\n", value);
#endif
    return value;
}

#define uart_periph_id2_read() uart_periph_id2_read_(device->main.resource.mem)


static inline int
uart_periph_id2_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_periph_id2_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_periph_id2:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_periph_id2_get_bit(value) uart_periph_id2_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_periph_id2_get_Designer1_(mem_space_t space) {
    uint32_t tmp = uart_periph_id2_read_(space);
    tmp = (tmp >> 0) & 0xfU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id2:Designer1 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id2_get_Designer1() uart_periph_id2_get_Designer1_(device->main.resource.mem)


static inline uint32_t
uart_periph_id2_get_Revision_(mem_space_t space) {
    uint32_t tmp = uart_periph_id2_read_(space);
    tmp = (tmp >> 4) & 0xfU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id2:Revision -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id2_get_Revision() uart_periph_id2_get_Revision_(device->main.resource.mem)

static inline void
uart_periph_id2_print_(mem_space_t space) {
    uint32_t tmp = uart_periph_id2_read_(space);
    printf("uart_periph_id2        : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	Designer1      : %" PRId32 "\n", ((tmp >> 0) & 0xfU));
    printf("	Revision       : %" PRId32 "\n", ((tmp >> 4) & 0xfU));
}

#define uart_periph_id2_print() uart_periph_id2_print_(device->main)
#define UART_PERIPH_ID3_CONFIGURATION(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uart_periph_id3_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xfec);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_periph_id3\n", value);
#endif
    return value;
}

#define uart_periph_id3_read() uart_periph_id3_read_(device->main.resource.mem)


static inline int
uart_periph_id3_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_periph_id3_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_periph_id3:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_periph_id3_get_bit(value) uart_periph_id3_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_periph_id3_get_Configuration_(mem_space_t space) {
    uint32_t tmp = uart_periph_id3_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id3:Configuration -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id3_get_Configuration() uart_periph_id3_get_Configuration_(device->main.resource.mem)

static inline void
uart_periph_id3_print_(mem_space_t space) {
    uint32_t tmp = uart_periph_id3_read_(space);
    printf("uart_periph_id3        : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	Configuration  : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uart_periph_id3_print() uart_periph_id3_print_(device->main)
#define UART_PCELL_ID0_UARTPCELLID0(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uart_pcell_id0_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xff0);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_pcell_id0\n", value);
#endif
    return value;
}

#define uart_pcell_id0_read() uart_pcell_id0_read_(device->main.resource.mem)


static inline int
uart_pcell_id0_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_pcell_id0_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_pcell_id0:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_pcell_id0_get_bit(value) uart_pcell_id0_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_pcell_id0_get_UARTPCellID0_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id0_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_pcell_id0:UARTPCellID0 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_pcell_id0_get_UARTPCellID0() uart_pcell_id0_get_UARTPCellID0_(device->main.resource.mem)

static inline void
uart_pcell_id0_print_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id0_read_(space);
    printf("uart_pcell_id0         : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	UARTPCellID0   : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uart_pcell_id0_print() uart_pcell_id0_print_(device->main)
#define UART_PCELL_ID1_UARTPCELLID1(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uart_pcell_id1_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xff4);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_pcell_id1\n", value);
#endif
    return value;
}

#define uart_pcell_id1_read() uart_pcell_id1_read_(device->main.resource.mem)


static inline int
uart_pcell_id1_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_pcell_id1_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_pcell_id1:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_pcell_id1_get_bit(value) uart_pcell_id1_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_pcell_id1_get_UARTPCellID1_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id1_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_pcell_id1:UARTPCellID1 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_pcell_id1_get_UARTPCellID1() uart_pcell_id1_get_UARTPCellID1_(device->main.resource.mem)

static inline void
uart_pcell_id1_print_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id1_read_(space);
    printf("uart_pcell_id1         : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	UARTPCellID1   : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uart_pcell_id1_print() uart_pcell_id1_print_(device->main)
#define UART_PCELL_ID2_UARTPCELLID2(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uart_pcell_id2_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xff8);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_pcell_id2\n", value);
#endif
    return value;
}

#define uart_pcell_id2_read() uart_pcell_id2_read_(device->main.resource.mem)


static inline int
uart_pcell_id2_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_pcell_id2_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_pcell_id2:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_pcell_id2_get_bit(value) uart_pcell_id2_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_pcell_id2_get_UARTPCellID2_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id2_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_pcell_id2:UARTPCellID2 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_pcell_id2_get_UARTPCellID2() uart_pcell_id2_get_UARTPCellID2_(device->main.resource.mem)

static inline void
uart_pcell_id2_print_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id2_read_(space);
    printf("uart_pcell_id2         : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	UARTPCellID2   : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uart_pcell_id2_print() uart_pcell_id2_print_(device->main)
#define UART_PCELL_ID3_UARTPCELLID3(value) (uint32_t) ((value & 0xffU) << 0)

static inline uint32_t
uart_pcell_id3_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xffc);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_pcell_id3\n", value);
#endif
    return value;
}

#define uart_pcell_id3_read() uart_pcell_id3_read_(device->main.resource.mem)


static inline int
uart_pcell_id3_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_pcell_id3_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_pcell_id3:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_pcell_id3_get_bit(value) uart_pcell_id3_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_pcell_id3_get_UARTPCellID3_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id3_read_(space);
    tmp = (tmp >> 0) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_pcell_id3:UARTPCellID3 -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_pcell_id3_get_UARTPCellID3() uart_pcell_id3_get_UARTPCellID3_(device->main.resource.mem)

static inline void
uart_pcell_id3_print_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id3_read_(space);
    printf("uart_pcell_id3         : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	UARTPCellID3   : %" PRId32 "\n", ((tmp >> 0) & 0xffU));
}

#define uart_pcell_id3_print() uart_pcell_id3_print_(device->main)
#define UART_PERIPH_ID_PARTNUMBER(value) (uint32_t) ((value & 0xfffU) << 0)
#define UART_PERIPH_ID_DESIGNER(value) (uint32_t) ((value & 0xffU) << 12)
#define UART_PERIPH_ID_REVISION(value) (uint32_t) ((value & 0xfU) << 20)
#define UART_PERIPH_ID_CONFIGURATION(value) (uint32_t) ((value & 0xffU) << 24)

static inline uint32_t
uart_periph_id_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xfe0);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_periph_id\n", value);
#endif
    return value;
}

#define uart_periph_id_read() uart_periph_id_read_(device->main.resource.mem)


static inline int
uart_periph_id_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_periph_id_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_periph_id:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_periph_id_get_bit(value) uart_periph_id_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_periph_id_get_PartNumber_(mem_space_t space) {
    uint32_t tmp = uart_periph_id_read_(space);
    tmp = (tmp >> 0) & 0xfffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id:PartNumber -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id_get_PartNumber() uart_periph_id_get_PartNumber_(device->main.resource.mem)


static inline uint32_t
uart_periph_id_get_Designer_(mem_space_t space) {
    uint32_t tmp = uart_periph_id_read_(space);
    tmp = (tmp >> 12) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id:Designer -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id_get_Designer() uart_periph_id_get_Designer_(device->main.resource.mem)


static inline uint32_t
uart_periph_id_get_Revision_(mem_space_t space) {
    uint32_t tmp = uart_periph_id_read_(space);
    tmp = (tmp >> 20) & 0xfU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id:Revision -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id_get_Revision() uart_periph_id_get_Revision_(device->main.resource.mem)


static inline uint32_t
uart_periph_id_get_Configuration_(mem_space_t space) {
    uint32_t tmp = uart_periph_id_read_(space);
    tmp = (tmp >> 24) & 0xffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_periph_id:Configuration -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_periph_id_get_Configuration() uart_periph_id_get_Configuration_(device->main.resource.mem)

static inline void
uart_periph_id_print_(mem_space_t space) {
    uint32_t tmp = uart_periph_id_read_(space);
    printf("uart_periph_id         : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	PartNumber     : %" PRId32 "\n", ((tmp >> 0) & 0xfffU));
    printf("	Designer       : %" PRId32 "\n", ((tmp >> 12) & 0xffU));
    printf("	Revision       : %" PRId32 "\n", ((tmp >> 20) & 0xfU));
    printf("	Configuration  : %" PRId32 "\n", ((tmp >> 24) & 0xffU));
}

#define uart_periph_id_print() uart_periph_id_print_(device->main)
#define UART_PCELL_ID_UARTPCELLID(value) (uint32_t) ((value & 0xffffffffU) << 0)

static inline uint32_t
uart_pcell_id_read_(mem_space_t space) {
    uint32_t value;
    value =  mem_space_read_32(space, 0xff0);
#if defined(DEBUG_DDL)
    printf("\t\tRead %x from register uart_pcell_id\n", value);
#endif
    return value;
}

#define uart_pcell_id_read() uart_pcell_id_read_(device->main.resource.mem)


static inline int
uart_pcell_id_get_bit_(mem_space_t space, uint8_t bit) {
    int tmp = uart_pcell_id_read_(space) & (1 << bit);
#if defined(DEBUG_DDL)
    printf("Getting bit uart_pcell_id:%d -- %d\
", bit, tmp);
#endif
    return tmp;
}

#define uart_pcell_id_get_bit(value) uart_pcell_id_get_bit_(device->main.resource.mem, value)

static inline uint32_t
uart_pcell_id_get_UARTPCellID_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id_read_(space);
    tmp = (tmp >> 0) & 0xffffffffU;
#if defined(DEBUG_DDL)
    printf("\tGetting field uart_pcell_id:UARTPCellID -- %x\n", tmp);
#endif
    return tmp;
}

#define uart_pcell_id_get_UARTPCellID() uart_pcell_id_get_UARTPCellID_(device->main.resource.mem)

static inline void
uart_pcell_id_print_(mem_space_t space) {
    uint32_t tmp = uart_pcell_id_read_(space);
    printf("uart_pcell_id          : 0x%08" PRIx32 " %s\n", tmp, fbin_32(tmp));
    printf("	UARTPCellID    : %" PRId32 "\n", ((tmp >> 0) & 0xffffffffU));
}

#define uart_pcell_id_print() uart_pcell_id_print_(device->main)
static inline void
main_print_(mem_space_t space) {
    printf("[Block main]\n");
    uartdr_print_(space);
    uartrsr_print_(space);
    uartecr_print_(space);
    uartfr_print_(space);
    uartilpr_print_(space);
    uartibrd_print_(space);
    uartfbrd_print_(space);
    uartlcr_h_print_(space);
    uartcr_print_(space);
    uartifls_print_(space);
    uartimsc_print_(space);
    uartris_print_(space);
    uartmis_print_(space);
    uartdmacr_print_(space);
    uart_periph_id0_print_(space);
    uart_periph_id1_print_(space);
    uart_periph_id2_print_(space);
    uart_periph_id3_print_(space);
    uart_pcell_id0_print_(space);
    uart_pcell_id1_print_(space);
    uart_pcell_id2_print_(space);
    uart_pcell_id3_print_(space);
    uart_periph_id_print_(space);
    uart_pcell_id_print_(space);
}

#define main_print() main_print_(device->main.resource.mem)
