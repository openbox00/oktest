#ifndef __L4__ARM__V5__PAGE_H__
#define __L4__ARM__V5__PAGE_H__

/* ARMv5 supports 4k, 64kb, 4mb,  */
#define USER_HW_VALID_PGSIZES ( \
        (1<<12) |               \
        (1<<16) |               \
        (1<<20))

#define __L4_MIN_PAGE_BITS      12

#define __L4_VALID_HW_PAGE_PERMS    \
       ((1 << 5) | /* R-X */        \
        (1 << 7))  /* RWX */

/* deprecated */
#define USER_HW_PAGE_PERMS      2
#define USER_MIN_PAGE_BITS      12

#endif /*__L4__ARM__V5__PAGE_H__*/
