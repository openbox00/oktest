/*
 * Description:   FASS inline functions (needed to break circular dependency)
 */

#ifndef __ARCH__ARM__FASS_INLINE_H__
#define __ARCH__ARM__FASS_INLINE_H__

#ifdef CONFIG_ENABLE_FASS

#include <arch/fass.h>
#include <space.h>

INLINE void arm_fass_t::add_set(arm_domain_t domain, word_t section)
{
}

INLINE void arm_fass_t::remove_set(arm_domain_t domain, word_t section)
{
}

INLINE void arm_fass_t::activate_domain(space_t *space)
{
    arm_domain_t target = space->get_domain();

    if (EXPECT_FALSE(target == INVALID_DOMAIN))
    {
        target = domain_recycle(space);
        /* Insert mapping to UTCBs into cpd */
        word_t section = ( UTCB_AREA_START/ARM_SECTION_SIZE +
                        (space->get_space_id().get_spaceno()) );
        pgent_t entry = *space->pgent_utcb();
        entry.set_domain(target);
        get_cpd()[section] = entry;
    }

    current_domain = target;
    current_domain_mask = space->get_domain_mask();

    domain_dirty |= current_domain_mask;
}

INLINE void arm_fass_t::activate_other_domain(space_t *space)
{
    arm_domain_t target = space->get_domain();

    if (EXPECT_FALSE(target == INVALID_DOMAIN))
    {
        target = domain_recycle(space);
        /* Insert mapping to UTCBs into cpd */
        word_t section = ( UTCB_AREA_START/ARM_SECTION_SIZE +
                        (space->get_space_id().get_spaceno()) );
        pgent_t entry = *space->pgent_utcb();
        entry.set_domain(target);
        get_cpd()[section] = entry;
    }
}

#endif

#endif /* __ARCH__ARM__FASS_INLINE_H__ */
