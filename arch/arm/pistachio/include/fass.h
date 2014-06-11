/*
 * Description:   FASS functions and defines
 */

#ifndef __ARCH__ARM__FASS_H__
#define __ARCH__ARM__FASS_H__

#define ARM_DOMAINS     16

#define KERNEL_DOMAIN   0
#define INVALID_DOMAIN ARM_DOMAINS

#define PID_AREA_SIZE   MB(32)

#if !defined(ASSEMBLY)

typedef unsigned int arm_domain_t;
typedef unsigned char arm_pid_t;

class space_t;


#define SET_DOMAIN(w,n)  ((w) = (w) | (1 << (n*2)))
#define TEST_DOMAIN(w,n) ((w) & (1 << (n*2)))

INLINE word_t * get_domain_dirty()
{
    return &get_arm_globals()->domain_dirty;
}

INLINE word_t * get_current_domain()
{
    return &get_arm_globals()->current_domain;
}

INLINE word_t * get_current_domain_mask()
{
    return &get_arm_globals()->current_domain_mask;
}

#define domain_dirty        (*get_domain_dirty())
#define current_domain      (*get_current_domain())
#define current_domain_mask (*get_current_domain_mask())

INLINE arm_fass_t * get_arm_fass() PURE;

INLINE arm_fass_t * get_arm_fass()
{
    return get_arm_globals()->arm_fass;
}

typedef unsigned int cpd_bitfield_t;

#define CPD_BITFIELD_WORD_SIZE (8*sizeof(cpd_bitfield_t))
#define CPD_BITFIELD_ARRAY_SIZE ((1 << ARM_SECTION_BITS) /  \
        CPD_BITFIELD_WORD_SIZE)

#define CPD_BITFIELD_POS(section) (section / CPD_BITFIELD_WORD_SIZE)
#define CPD_BITFIELD_OFFSET(section) (section % CPD_BITFIELD_WORD_SIZE)

class arm_fass_t {
public:
    void init(void);
    void clean_all( word_t flush );

    void add_set( arm_domain_t domain, word_t section );
    void remove_set( arm_domain_t domain, word_t section );

    void activate_domain( space_t *space );
    void activate_other_domain( space_t *space );
    void free_domain( space_t *space );

    space_t *get_space( arm_domain_t domain );
    void set_space( arm_domain_t domain, space_t *space );

private:
    int replacement_domain(void);
    arm_domain_t domain_recycle(space_t *space);

private:
    cpd_bitfield_t cpd_set[ARM_DOMAINS][CPD_BITFIELD_ARRAY_SIZE];
    space_t *domain_space[ARM_DOMAINS];
    unsigned int rand;
};

#endif

#endif /* __ARCH__ARM__FASS_H__ */
