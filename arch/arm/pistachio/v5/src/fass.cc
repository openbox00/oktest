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
    domain_dirty = 0;
    this->domain_space[0] = get_kernel_space();
    get_kernel_space()->set_domain(0);
    rand = 1;
}

void arm_fass_t::clean_all(word_t flush)
{
    if (flush) {
        domain_dirty = current_domain_mask;
        arm_cache::cache_flush();
    }

    arm_cache::tlb_flush();
}

/* Choose a domain to replace - try select a clean domain, otherwise replace one.
 * Should consider using a more sophisticated selection if
 * it will buy anything (for example, consider the number of CPD slots used by
 * the domains). Also, should consider moving this to user-level.
 */

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
    ASSERT(DEBUG, target != current_domain);

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
    if (!space)
        return;

    arm_domain_t target;

    /* remove access to this domain */
    space->flush_sharing_spaces();

    target = space->get_domain();
    space->set_domain(INVALID_DOMAIN);
    domain_space[target] = NULL;

    /*
     * Remove the elements in the CPD belonging to the domain to be
     * removed.
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
}

space_t *arm_fass_t::get_space(arm_domain_t domain)
{
    if (domain == INVALID_DOMAIN)
        return NULL;

    return domain_space[domain];
}

void arm_fass_t::set_space(arm_domain_t domain, space_t *space)
{
    if (domain != INVALID_DOMAIN)
        domain_space[domain] = space;
}



#endif /* CONFIG_ENABLE_FASS */
