/*
 * Description:   FASS functionality
 */

#ifdef CONFIG_ENABLE_FASS

#include <l4.h>
#include <space.h>
#include <arch/fass.h>
#include <kernel/arch/special.h>
#include <tcb.h>

arm_fass_t arm_fass;

void arm_fass_t::init(void)
{
}

void arm_fass_t::clean_all(word_t flush)
{
}

int arm_fass_t::replacement_domain(void)
{
    word_t domain;
    rand += 7333;

    /* First search for a clean domain */
    word_t clean = (~domain_dirty) & 0x55555555;

    if (EXPECT_TRUE(clean)) {
        domain = msb(clean);
        if (EXPECT_TRUE(domain)) {  /* Don't select domain 0 */
            return domain/2;
        }
    }

    /* Pick the next available domain */
    do {
        rand = rand + (rand << 7) + 6277;
        rand = (rand << 7) ^ (rand >> 17) ^ (rand << 11);

        domain = rand % ARM_DOMAINS;
    } while ((domain == 0) || (domain == current_domain));
    return domain;
}

arm_domain_t arm_fass_t::domain_recycle(space_t *space)
{

    arm_domain_t target;
    space_t *old_space;

    target = replacement_domain();

    old_space = domain_space[target];

    if (old_space)
    {
        /* remove access to this domain */
        old_space->flush_sharing_spaces();
        old_space->set_domain(INVALID_DOMAIN);
    }

    space->set_domain(target);

    domain_space[target] = space;

    /*
     * Remove the elements in the CPD belonging to the domain to be
     * recycled.
     */
    pgent_t *cpd = get_cpd();
    cpd_bitfield_t *domain_set = cpd_set[target];
    for (word_t i = 0, j; i < CPD_BITFIELD_ARRAY_SIZE*CPD_BITFIELD_WORD_SIZE; i+=CPD_BITFIELD_WORD_SIZE) {
        j = *domain_set ++;

        if (EXPECT_FALSE(j))
        {
            do {
                word_t k = msb(j);

                word_t section = i + k;
                cpd[section].raw = 0;

                j &= ~(1UL << k);
            } while (j);

            *(domain_set-1) = 0;
        }
    }

    if (old_space) {
        bool dirty = TEST_DOMAIN(domain_dirty, target);

        /* Remove mapping to UTCBs from old_space */
        word_t section = ( UTCB_AREA_START/ARM_SECTION_SIZE +
                        (old_space->get_space_id().get_spaceno()) );
        cpd[section].raw = (word_t)old_space;

        clean_all(dirty);
    }
    return target;
}

/* Free up a domain, space must have a valid domain */

void arm_fass_t::free_domain(space_t *space)
{
}

space_t *arm_fass_t::get_space(arm_domain_t domain)
{
    if (domain == INVALID_DOMAIN)
        return NULL;

    return domain_space[domain];
}


#endif /* CONFIG_ENABLE_FASS */
