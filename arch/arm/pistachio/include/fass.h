#ifndef __ARCH__ARM__FASS_H__
#define __ARCH__ARM__FASS_H__

#define ARM_DOMAINS     16
#define KERNEL_DOMAIN   0
#define INVALID_DOMAIN ARM_DOMAINS
typedef unsigned int arm_domain_t;
typedef unsigned char arm_pid_t;

INLINE word_t * get_current_domain()
{
    return &get_arm_globals()->current_domain;
}

#define domain_dirty        (*get_domain_dirty())
#define current_domain      (*get_current_domain())
#define current_domain_mask (*get_current_domain_mask())

INLINE arm_fass_t * get_arm_fass() PURE;
INLINE arm_fass_t * get_arm_fass()
{
    return get_arm_globals()->arm_fass;
}

class arm_fass_t {
public:
    void activate_other_domain( space_t *space );
};

#endif /* __ARCH__ARM__FASS_H__ */
