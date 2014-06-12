/*
 * Description:   Functions which manipulate the ARM926EJ cache
 */

#ifndef __ARCH__ARM__ARM926EJ_CACHE_H_
#define __ARCH__ARM__ARM926EJ_CACHE_H_

#include <debug.h>
#include <cpu/syscon.h>
#include <kernel/arch/asm.h>

/* Write-through mode on arm926 */
#define CPU_WRITETHROUGH        2

class generic_space_t;

/*
 * XXX mothra/issues/3043
 */
#if defined(CONFIG_PLAT_STN8815)
#define ICACHE_SIZE      (16 * 1024)
#else
#define ICACHE_SIZE      (32 * 1024)
#endif

#define ICACHE_LINE_SIZE 32
#define ICACHE_WAYS      4
#define ICACHE_SETS     (ICACHE_SIZE / ICACHE_WAYS / ICACHE_LINE_SIZE)
#define DCACHE_SIZE      ICACHE_SIZE
#define DCACHE_LINE_SIZE ICACHE_LINE_SIZE
#define DCACHE_WAYS      ICACHE_WAYS
#define DCACHE_SETS      ICACHE_SETS

#define CACHE_LINE_MASK (DCACHE_LINE_SIZE - 1)

#define ARM_CPU_HAS_TLBLOCK     1

/* XXX: These are seedy macros which lets us share code between gcc and rvct */
#if defined(__GNUC__)
#define LOOP_LABEL      "1:"
#define LOOP_TARGET     "1b"
#elif defined(__RVCT_GNU__)
#define LOOP_LABEL      "L1:"
#define LOOP_TARGET     "L1"
/* In gcc inline asm, operands are symbolic and mapped to variables in
 * a separate part, nor can the names contain '::'.  For RVCT the
 * operands are the actual variables.  Therefore for RVCT directly map
 * the symbolic name to the variable found in the platform namespace.
 */
#define ASM_CACHE_LINE_SIZE DCACHE_LINE_SIZE
#elif defined(_lint)
#else
#error "Can't handle inline assembler for selected toolchain"
#endif

class arm_cache
{
public:
    static bool full_flush_cheaper(word_t size)
    {
        return (size >= DCACHE_SIZE ? true : false);
    }

    static inline void cache_flush(void)
    {
        cache_flush_d();
        cache_flush_i();
    }
    static inline void cache_flush_d(void)
    {
        /* test - clean and invalidate */
        __asm__ __volatile__ (
            LOOP_LABEL"                         ;"
            "   mrc     p15, 0, r15, c7, c14, 3 ;"
            "   bne     "LOOP_TARGET"           ;"
        );

        cache_drain_write_buffer();
    }

    static inline void cache_flush_i(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
             "  mcr     p15, 0, "_(zero)", c7, c5, 0    ;"      /* Flush I cache */
            :: [zero] "r" (zero)
        );
    }

    static inline void cache_flush_i_ent(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {
            cache_flush_i();
            return;
        }
        word_t size = 1UL << log2size;

        cache_invalidate_ilines(vaddr, size);
    }

    static inline void cache_flush_d_ent(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {
                cache_flush();
                return;
        }
        word_t size = 1UL << log2size;

        cache_clean_invalidate_dlines(vaddr, size);
    }

    static inline void cache_flush_ent(addr_t vaddr, word_t log2size)
    {
        if (log2size >= 15) {
                cache_flush();
                return;
        }
        word_t size = 1UL << log2size;

        __asm__ __volatile__ (
            "    add     r1, "_(vaddr)", "_(size)"                      ;"
            LOOP_LABEL"                                                 ;"
            /* clean + invalidate D cache line */
            "    mcr     p15, 0, "_(vaddr)", c7, c14, 1                 ;"
            /* flush I cache line */
            "    mcr     p15, 0, "_(vaddr)", c7, c5, 1                  ;"
            "    add     "_(vaddr)", "_(vaddr)", "_(ASM_CACHE_LINE_SIZE)"   ;"
            "    cmp     r1, "_(vaddr)"                                 ;"
            "    bhi     "LOOP_TARGET"                                  ;"
            "    mov     "_(vaddr)",     #0                             ;"
            /* drain write buffer */
            "    mcr     p15, 0, "_(vaddr)", c7, c10, 4                 ;"
            : [vaddr] "+r" (vaddr)
            : [size] "r" (size), [ASM_CACHE_LINE_SIZE] "r" (DCACHE_LINE_SIZE)
            : "r1", "memory"
            );
    }

    /* These require CACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_clean_invalidate_dlines(addr_t vaddr, word_t size)
    {
        __asm__ __volatile__ (
            "    add     r1, "_(vaddr)", "_(size)"                    ;"
            LOOP_LABEL"                                               ;"
            /* clean + invalidate D cache line */
            "    mcr     p15, 0, "_(vaddr)", c7, c14, 1               ;"
            "    add     "_(vaddr)", "_(vaddr)", "_(ASM_CACHE_LINE_SIZE)" ;"
            "    cmp     r1,     "_(vaddr)"                           ;"
            "    bhi     "LOOP_TARGET"                                ;"
            "    mov     "_(vaddr)",     #0                           ;"
            /* drain write buffer */
            "    mcr     p15, 0, "_(vaddr)", c7, c10, 4               ;"
            : [vaddr] "+r" (vaddr)
            : [size] "r" (size), [ASM_CACHE_LINE_SIZE] "r" (DCACHE_LINE_SIZE)
            : "r1", "memory"
            );
    }

    /* These require CACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_clean_dlines(addr_t vaddr, word_t size)
    {
        __asm__ __volatile__ (
            "    add     r1,     "_(vaddr)",    "_(size)"               ;"
            LOOP_LABEL"                                                 ;"
            /* clean D cache line */
            "    mcr     p15, 0, "_(vaddr)", c7, c10, 1                 ;"
            "    add     "_(vaddr)", "_(vaddr)", "_(ASM_CACHE_LINE_SIZE)"   ;"
            "    cmp     r1, "_(vaddr)"                                 ;"
            "    bhi     "LOOP_TARGET"                                  ;"
            "    mov     "_(vaddr)",     #0                             ;"
            /* drain write buffer */
            "    mcr     p15, 0, "_(vaddr)", c7, c10, 4                 ;"
            : [vaddr] "+r" (vaddr)
            : [size] "r" (size), [ASM_CACHE_LINE_SIZE] "r" (DCACHE_LINE_SIZE)
            : "r1", "memory"
            );
    }

    /* These require CACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_invalidate_dlines(addr_t vaddr, word_t size)
    {
        __asm__ __volatile__ (
            "    add     r1, "_(vaddr)", "_(size)"                    ;"
            LOOP_LABEL"                                               ;"
            /* invalidate D cache line */
            "    mcr     p15, 0, "_(vaddr)", c7, c6, 1                ;"
            "    add     "_(vaddr)", "_(vaddr)", "_(ASM_CACHE_LINE_SIZE)" ;"
            "    cmp     r1, "_(vaddr)"                               ;"
            "    bhi     "LOOP_TARGET"                                ;"
            "    mov     "_(vaddr)",     #0                           ;"
            /* drain write buffer */
            "    mcr     p15, 0, "_(vaddr)", c7, c10, 4               ;"
            : [vaddr] "+r" (vaddr)
            : [size] "r" (size), [ASM_CACHE_LINE_SIZE] "r" (DCACHE_LINE_SIZE)
            : "r1", "memory"
            );
    }

    /* These require CACHE_LINE_SIZE aligned addresses/size */
    static inline void cache_invalidate_ilines(addr_t vaddr, word_t size)
    {
        __asm__ __volatile__ (
            "    add     r1,     "_(vaddr)",    "_(size)"             ;"
            LOOP_LABEL"                                               ;"
            /* invalidate I cache line */
            "    mcr     p15, 0, "_(vaddr)", c7, c5, 1                ;"
            "    add     "_(vaddr)", "_(vaddr)", "_(ASM_CACHE_LINE_SIZE)" ;"
            "    cmp     r1,     "_(vaddr)"                           ;"
            "    bhi     "LOOP_TARGET"                                ;"
            "    mov     "_(vaddr)",     #0                           ;"
            :[vaddr]  "+r" (vaddr)
            : [size] "r" (size), [ASM_CACHE_LINE_SIZE] "r" (DCACHE_LINE_SIZE)
            : "r1", "memory"
            );
    }

    static inline void cache_flush_range_attr(generic_space_t *space,
            addr_t vaddr, word_t size, word_t attr)
    {
        size = ((size + DCACHE_LINE_SIZE-1) + ((word_t)vaddr & (DCACHE_LINE_SIZE-1)))
            & (~(DCACHE_LINE_SIZE-1));
        vaddr = (addr_t)((word_t)vaddr & (~(DCACHE_LINE_SIZE-1)));               // align addresses
    }

    static inline void tlb_flush(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, "_(zero)", c8, c7, 0   ;"      /* flush I+D TLB */
            :: [zero] "r" (zero)
        );
    }

    static inline void tlb_flush_ent(addr_t vaddr, word_t log2size)
    {
        /*lint -esym(550,a) */
        word_t a = (word_t)vaddr;

        for (word_t i=0; i < (1UL << log2size); i += ARM_PAGE_SIZE)
        {
            __asm__ __volatile__ (
                /* Invalidate I TLB entry */
                "    mcr     p15, 0, "_(a)", c8, c5, 1    ;"
                /* Invalidate D TLB entry */
                "    mcr     p15, 0, "_(a)", c8, c6, 1    ;"
            :: [a] "r" (a)
            );
            a += ARM_PAGE_SIZE;
        }
    }

    static inline void cache_enable()
    {
        __asm__ __volatile__ (
            "   mov    r0, #0                    ;"
            "   mcr    p15, 0, r0, c7, c10, 4    ;"     // Drain the pending operations
            "   mrc    p15, 0, r0, c1, c0, 0     ;"     // Get the control register
            "   orr    r0, r0, #0x1004           ;"     // Set bit 12 - the I & D bit
            "   mcr    p15, 0, r0, c1, c0, 0     ;"     // Set the control register
            ::: "r0"
        );
    }

    static inline void cache_invalidate_d_line(word_t target)
    {
        __asm__ __volatile__ (
            "   mcr p15, 0, "_(target)", c7, c6, 1 ;"
        :: [target] "r" (target)
        );
    }

    static inline void cache_invalidate_i()
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, "_(zero)", c7, c5, 0   ;"
            :: [zero] "r" (zero)
        );
    }

    static inline void cache_invalidate_d()
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, "_(zero)", c7, c6, 0   ;"
            :: [zero] "r" (zero)
        );
    }

    static inline void cache_clean(word_t target)
    {
        __asm__ __volatile__ (
            "   mcr p15, 0, "_(target)", c7, c10, 1 ;"
        :: [target] "r" (target)
        );
    }

    static inline void cache_drain_write_buffer(void)
    {
        word_t zero = 0;
        __asm__ __volatile__ (
            "   mcr     p15, 0, "_(zero)", c7, c10, 4   ;"
            :: [zero] "r" (zero)
        );
    }

    static inline void prefetch_icache(addr_t vaddr)
    {
        __asm__ __volatile__ (
                "   mcr     p15, 0, " _(vaddr) ", c7, c13, 1"   /* prefetch icache line */
                :: [vaddr] "r" (vaddr)
        );
    }

    static inline void lock_tlb_addr(addr_t vaddr)
    {
        __asm__ __volatile__ (
                "   mrc     p15, 0, r0, c10, c0, 0          ;"  /* Get lockdown register    */
                "   orr     r0, r0, #1                      ;"  /* Set preserve bit         */
                "   mcr     p15, 0, r0, c10, c0, 0          ;"  /* Write lockdown register  */
                "   mcr     p15, 0, " _(vaddr) ", c8, c7, 1 ;"  /* - Invalidate TLB entry   */
                "   ldr     r0, [" _(vaddr) "]              ;"  /* - Touch the entry        */
                "   mrc     p15, 0, r0, c10, c0, 0          ;"  /* Get lockdown register    */
                "   bic     r0, r0, #1                      ;"  /* Clear preserve bit       */
                "   mcr     p15, 0, r0, c10, c0, 0          ;"  /* Write lockdown register  */
                :: [vaddr] "r" (vaddr)
                : "r0"
        );
    }

    static inline void unlock_icache()
    {
        word_t unlock = 0x0000fff0;

        __asm__ __volatile__ (
                "       mcr     p15, 0, " _(unlock) ", c9, c0, 1"       /* Set the value of c9 (i-cache lockdown register)      */
                :: [unlock] "r" (unlock)
        );
    }

    static inline void unlock_dcache()
    {
        word_t unlock = 0x0000fff0;

        __asm__ __volatile__ (
                "       mcr     p15, 0, " _(unlock) ", c9, c0, 0"       /* Set the value of c9 (d-cache lockdown register)      */
                :: [unlock] "r" (unlock)
        );
    }

    /* Lock address range from vstart to vstart+size-1 in cache way (way) [0..3]
     * Must be called from uncached code */
    static inline void lock_icache_range(addr_t vstart, word_t size, word_t way)
    {
        word_t save;
        word_t way_bit = (1 << way);
        word_t lock = 0x0000fff0 | (0xf ^ way_bit);
        word_t vend = (word_t)vstart + size;

        __asm__ __volatile__ (
                "       mrc     p15, 0, " _(save) ", c9, c0, 1          ;"      /* Save the value of c9 (i-cache lockdown register)     */
                "       orr     " _(save) ", " _(save) ", " _(way_bit) ";"      /* Set the lock bit for selected way                    */
                "       mcr     p15, 0, " _(lock) ", c9, c0, 1          ;"      /* Set the value of c9 (i-cache lockdown register)      */

                LOOP_LABEL "                                            ;"
                "       bic     " _(vstart) ", " _(vstart) ",   #31     ;"
                "       mcr     p15, 0, " _(vstart) ", c7, c13, 1       ;"      /* prefetch icache line */
                "       add     " _(vstart) ", " _(vstart) ",   #32     ;"
                "       cmp     " _(vstart) ", " _(vend) "              ;"
                "       bls     " LOOP_TARGET "                         ;"

                "       mrc     p15, 0, " _(lock) ", c9, c0, 1          ;"      /* Save the value of c9 (i-cache lockdown register)     */
                "       nop                                             ;"
                "       mcr     p15, 0, " _(save) ", c9, c0, 1          ;"      /* Restore the value of c9 (i-cache lockdown register)  */
                : [save] "+r" (save), [lock] "+r" (lock),
                [way_bit] "+r" (way_bit), [vstart] "+r" (vstart),
                [vend] "+r" (vend)
                );
    }

    /* Lock address range from vstart to vstart+size-1 in cache way (way) [0..3]
     * Must be called from uncached code */
    static inline void lock_dcache_range(addr_t vstart, word_t size, word_t way)
    {
        word_t save;
        word_t way_bit = (1 << way);
        word_t lock = 0x0000fff0 | (0xf ^ way_bit);
        word_t vend = (word_t)vstart + size;

        __asm__ __volatile__ (
                "       mrc     p15, 0, " _(save) ", c9, c0, 0          ;"      /* Save the value of c9 (d-cache lockdown register)     */
                "       orr     " _(save) ", " _(save) ", " _(way_bit) ";"      /* Set the lock bit for selected way                    */
                "       mcr     p15, 0, " _(lock) ", c9, c0, 0          ;"      /* Set the value of c9 (d-cache lockdown register)      */

                LOOP_LABEL "                                            ;"
                "       bic     " _(vstart) ", " _(vstart) ",   #31     ;"
                "       ldr     " _(way_bit) ", [" _(vstart) "]         ;"      /* load dcache line */
                "       add     " _(vstart) ", " _(vstart) ",   #32     ;"
                "       cmp     " _(vstart) ", " _(vend) "              ;"
                "       bls     " LOOP_TARGET "                         ;"

                "       mrc     p15, 0, " _(lock) ", c9, c0, 0          ;"      /* Save the value of c9 (d-cache lockdown register)     */
                "       nop                                             ;"
                "       mcr     p15, 0, " _(save) ", c9, c0, 0          ;"      /* Restore the value of c9 (d-cache lockdown register)  */
                : [save] "+r" (save), [lock] "+r" (lock),
                [way_bit] "+r" (way_bit), [vstart] "+r" (vstart),
                [vend] "+r" (vend)
                );
    }
    
    static inline void cache_invalidate_lines(addr_t vaddr, word_t nbytes)
    {
        // vaddr and nbytes must be aligned at cache line boundary
        __asm__ __volatile__ (
                LOOP_LABEL "                                                        ;"
                "   mcr         p15, 0, " _(vaddr) ", c7, c5, 1                     ;"
                "   mcr         p15, 0, " _(vaddr) ", c7, c6, 1                     ;"
                "   add         " _(vaddr) ", " _(vaddr) ", " _(ASM_CACHE_LINE_SIZE) "  ;"
                "   subs        " _(nbytes) ", " _(nbytes) ", " _(ASM_CACHE_LINE_SIZE) ";"
                "   bgt         " LOOP_TARGET "                                     ;"
                :: [vaddr] "r" (vaddr), [nbytes] "r" (nbytes),
                 [ASM_CACHE_LINE_SIZE] "i" (DCACHE_LINE_SIZE)
        );
    }

    static inline void cache_for_rewrite_bytecode(addr_t vaddr, word_t nbytes)
    {
        nbytes += (word_t)vaddr & CACHE_LINE_MASK;
        vaddr = (addr_t) ((word_t)vaddr & ~CACHE_LINE_MASK);

        cache_invalidate_lines(vaddr, nbytes);
        cache_drain_write_buffer();
    }

};

#endif /* __ARCH__ARM__ARM926EJ_CACHE_H_ */
