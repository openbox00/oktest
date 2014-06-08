/**
 * @file
 * @brief The OKL4 cache attributes.
 */
#ifndef __LIB_KERNEL_CACHE_H__
#define __LIB_KERNEL_CACHE_H__

#define CACHE_ATTRIB_OP_CLEAN           (1<<2)
#define CACHE_ATTRIB_OP_INVAL           (2<<2)
#define CACHE_ATTRIB_OP_CLEAN_INVAL     (3<<2)
#define CACHE_ATTRIB_OP_MASK            (3<<2)

#define CACHE_ATTRIB_I(x)               ((x) | 1)
#define CACHE_ATTRIB_D(x)               ((x) | 2)
#define CACHE_ATTRIB_ID(x)              ((x) | 3)
#define CACHE_ATTRIB_MASK               (3)

/* Masks */
#define CACHE_ATTRIB_MASK_I             CACHE_ATTRIB_I(0)
#define CACHE_ATTRIB_MASK_D             CACHE_ATTRIB_D(0)
#define CACHE_ATTRIB_MASK_ID            CACHE_ATTRIB_ID(0)
#define CACHE_ATTRIB_MASK_I_OP          CACHE_ATTRIB_I(CACHE_ATTRIB_OP_MASK)
#define CACHE_ATTRIB_MASK_D_OP          CACHE_ATTRIB_D(CACHE_ATTRIB_OP_MASK)
#define CACHE_ATTRIB_CLEAN_I            CACHE_ATTRIB_I(CACHE_ATTRIB_OP_CLEAN)
#define CACHE_ATTRIB_INVAL_I            CACHE_ATTRIB_I(CACHE_ATTRIB_OP_INVAL)
#define CACHE_ATTRIB_CLEAN_INVAL_I      CACHE_ATTRIB_I(CACHE_ATTRIB_OP_CLEAN_INVAL)
#define CACHE_ATTRIB_CLEAN_D            CACHE_ATTRIB_D(CACHE_ATTRIB_OP_CLEAN)
#define CACHE_ATTRIB_INVAL_D            CACHE_ATTRIB_D(CACHE_ATTRIB_OP_INVAL)
#define CACHE_ATTRIB_CLEAN_INVAL_D      CACHE_ATTRIB_D(CACHE_ATTRIB_OP_CLEAN_INVAL)

enum cacheattr_e {
    attr_invalidate_i   = CACHE_ATTRIB_I (CACHE_ATTRIB_OP_INVAL),
    attr_invalidate_d   = CACHE_ATTRIB_D (CACHE_ATTRIB_OP_INVAL),
    attr_invalidate_id  = CACHE_ATTRIB_ID(CACHE_ATTRIB_OP_INVAL),
    attr_clean_i        = CACHE_ATTRIB_I (CACHE_ATTRIB_OP_CLEAN),
    attr_clean_d        = CACHE_ATTRIB_D (CACHE_ATTRIB_OP_CLEAN),
    attr_clean_id       = CACHE_ATTRIB_ID(CACHE_ATTRIB_OP_CLEAN),
    attr_clean_inval_i  = CACHE_ATTRIB_I (CACHE_ATTRIB_OP_CLEAN_INVAL),
    attr_clean_inval_d  = CACHE_ATTRIB_D (CACHE_ATTRIB_OP_CLEAN_INVAL),
    attr_clean_inval_id = CACHE_ATTRIB_ID(CACHE_ATTRIB_OP_CLEAN_INVAL),
};


#endif
