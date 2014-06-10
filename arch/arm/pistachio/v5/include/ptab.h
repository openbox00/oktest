/*
 * Description:   ARM page table structures
 */
#ifndef __ARCH__ARM__PTAB_H__
#define __ARCH__ARM__PTAB_H__


#include <l4/arch/cache_attribs.h>

/*
 * The defines CACHE_ATTRIB_* are defined in libs/l4, in the appropriate
 * arch - ver - cpu cache_attribs.h include file.
 */

enum memattrib_e {
    writeback           = CACHE_ATTRIB_WRITEBACK,
    cached              = writeback,
    uncached            = CACHE_ATTRIB_UNCACHED,
    iomemory            = uncached, /* Alias on this architecture. */
    writecombine        = CACHE_ATTRIB_WRITECOMBINE,
    writethrough        = CACHE_ATTRIB_WRITETHROUGH,
#if defined(CONFIG_DEFAULT_CACHE_ATTR_WB)
    l4default           = cached,
#else
    l4default           = writethrough, /* NB: this will affect performance */
#endif
};

class arm_l2_desc_t
{
public:
    enum ap_e {
        special = 0,
        none    = 1,
        ro      = 2,
        rw      = 3,
    };

    union {
        struct {
            BITFIELD2(word_t,
                zero            : 2,
                ign             : 30
                      );
        } fault;

        struct {
            BITFIELD5(word_t,
                one             : 2,
                attrib          : 2,
                ap              : 8,
                sbz             : 4,
                base_address    : 16
                      );
        } large;

        struct {
            BITFIELD4(word_t,
                two             : 2,
                attrib          : 2,
                ap              : 8,
                base_address    : 20
                      );
        } small;

        struct {
            BITFIELD5(word_t,
                three           : 2,
                attrib          : 2,
                ap              : 2,
                sbz             : 4,
                base_address    : 22
                      );
        } tiny;

        u32_t raw;
    };

    // Predicates
    bool is_valid (void)
        { return fault.zero != 0; }

    memattrib_e attributes (void)
        { return (memattrib_e)large.attrib; }

    void clear()
        { raw = 0; }

    addr_t address_large()
        { return (addr_t)(large.base_address << 16); }

    addr_t address_small()
        { return (addr_t)(small.base_address << 12); }

    addr_t address_tiny()
        { return (addr_t)(tiny.base_address << 10); }
};


class arm_l1_desc_t
{
public:
    enum ap_e {
        special = 0,
        none    = 1,
        ro      = 2,
        rw      = 3,
    };

    union {
        struct {
            BITFIELD2(word_t,
                zero            : 2,
                ign             : 30
                      );
        } fault;

        struct {
            BITFIELD5(word_t,
                one             : 2,
                imp             : 3,
                domain          : 4,
                sbz             : 1,
                base_address    : 22
                      );
        } coarse_table;

        struct {
            BITFIELD8(word_t,
                two             : 2,
                attrib          : 2,
                imp             : 1,
                domain          : 4,
                sbz1            : 1,
                ap              : 2,
                sbz2            : 8,
                base_address    : 12
                      );
        } section;

        struct {
            BITFIELD5(word_t,
                three           : 2,
                imp             : 3,
                domain          : 4,
                sbz             : 3,
                base_address    : 20
                      );
        } fine_table;

        u32_t raw;
    };

    // Predicates

    bool is_valid (void)
        { return fault.zero != 0; }

    memattrib_e attributes (void)
        { return (memattrib_e)section.attrib; }

    void clear()
        { raw = 0; }

    addr_t address_section()
        { return (addr_t)(section.base_address << 20); }

    word_t address_finetable()
        { return (fine_table.base_address << 12); }

    word_t address_coarsetable()
        { return (coarse_table.base_address << 10); }
};

#define ARM_L0_BITS             ((32 -ARM_TOP_LEVEL_BITS) + 2)
#define ARM_L1_BITS             (ARM_SECTION_BITS - (32 - ARM_TOP_LEVEL_BITS) + 2)
#define ARM_L2_NORMAL_BITS      (32 - ARM_SECTION_BITS - PAGE_BITS_4K + 2)
#define ARM_L2_TINY_BITS        (32 - ARM_SECTION_BITS - PAGE_BITS_1K + 2)
#define ARM_HWL1_BITS           (ARM_SECTION_BITS + 2)

#define ARM_L0_ENTRIES          ( 1 << (ARM_L0_BITS - 2))
#define ARM_L1_ENTRIES          ( 1 << (ARM_L1_BITS - 2))
#define ARM_L2_ENTRIES_NORMAL   ( 1 << (ARM_L2_NORMAL_BITS - 2))
#define ARM_L2_ENTRIES_TINY     ( 1 << (ARM_L2_TINY_BITS - 2))
#define ARM_HWL1_ENTRIES                ( 1 << (ARM_HWL1_BITS - 2))

#define ARM_L0_SIZE             (1 << ARM_L0_BITS)
#define ARM_L1_SIZE             (1 << ARM_L1_BITS)
#define ARM_L2_SIZE_NORMAL      (1 << ARM_L2_NORMAL_BITS)
#define ARM_L2_SIZE_TINY        (1 << ARM_L2_TINY_BITS)
#define ARM_HWL1_SIZE           (1 << ARM_HWL1_BITS)

#define PG_TOP_SIZE ARM_L0_SIZE

#endif /* !__ARCH__ARM__PTAB_H__ */
