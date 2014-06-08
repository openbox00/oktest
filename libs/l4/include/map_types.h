/*
 * Description:   Interfaces for mappings in map control
 */
#ifndef __L4__MAP_TYPES_H__
#define __L4__MAP_TYPES_H__

#define __L4_MAP_SHIFT        10

typedef struct {
    BITFIELD4(word_t,
        attr        : 8,
        replace     : 1,
        sparse      : 1,
        vbase       : WORD_T_BIT - 10);
} virtual_descriptor_t;

typedef struct {
    BITFIELD4(word_t,
        rwx         : 3,
        valid       : 1,
        page_size   : 6,
        num_pages   : WORD_T_BIT - 10);
} size_descriptor_t;

typedef struct {
    BITFIELD2(word_t,
        segment     : 10,
        offset      : WORD_T_BIT - 10);
} segment_descriptor_t;

#endif /* !__L4__MAP_TYPES_H__ */
