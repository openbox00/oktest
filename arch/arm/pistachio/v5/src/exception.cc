/*
 * Description:   ARMv5 syscall and page fault handlers
 */
#include <space.h>
#include <tcb.h>


bool fass_sync_address (space_t * space, addr_t fault_addr, bool * is_valid = NULL);
void fass_update_pds (space_t * space, pgent_t cpg, pgent_t spg, word_t fault_section);

extern "C" void arm_memory_abort(word_t fault_status, addr_t fault_addr,
        arm_irq_context_t *context, word_t data_abt)
{
    tcb_t * current = get_current_tcb();

    space_t *space = current->get_space();

    bool is_kernel = ((context->cpsr & CPSR_MODE_MASK) != CPSR_USER_MODE);
    word_t fault_section = (word_t)fault_addr >> PAGE_BITS_1M;

    if (EXPECT_FALSE(is_kernel)) {
        /* Special case when UTCB sections faulted on */
        if (((word_t)fault_addr >= UTCB_AREA_START) &&
                ((word_t)fault_addr < UTCB_AREA_END))
        {
            pgent_t cpd_entry = get_cpd()[fault_section];

            if (cpd_entry.raw && ((cpd_entry.raw & 0xf0000003) == 0xf0000000)) {
                space_t * utcb_space = (space_t*)cpd_entry.raw;
                get_arm_fass()->activate_other_domain(utcb_space);
                return;
            }
            return;
        }
    }

    bool is_valid = false;
    if (space->is_user_area(fault_addr))
    {
        arm_domain_t fault_domain;
        pgent_t cpd_entry = get_cpd()[fault_section];
        fault_domain = cpd_entry.get_domain();

        if (fault_domain != current_domain)
        {
            pgent_t *pg = space->pgent((word_t)fault_addr >> PAGE_BITS_TOP_LEVEL);
            if (pg->is_valid(space, pgent_t::size_level0))
            {
                pgent_t section = *(pg->subtree(space, pgent_t::size_level0)->next(
                            space, pgent_t::size_1m,
                            ((word_t)fault_addr & (PAGE_SIZE_TOP_LEVEL-1)) >> PAGE_BITS_1M));

                if (EXPECT_FALSE(section.is_window(space, pgent_t::size_1m)))
                {
                    space_t *share_space = (space_t*)section.get_window(space);
                    pgent_t *share_pg = share_space->pgent((word_t)fault_addr >> PAGE_BITS_TOP_LEVEL);


                    if (share_pg->is_valid(share_space, pgent_t::size_level0))
                    {
                        pgent_t share_sec = *(share_pg->subtree(share_space, pgent_t::size_level0)->next(
                                    share_space, pgent_t::size_1m,
                                    ((word_t)fault_addr & (PAGE_SIZE_TOP_LEVEL-1)) >> PAGE_BITS_1M));

                        if (share_sec.is_valid(share_space, pgent_t::size_1m))
                        {
                            arm_domain_t share_domain = share_space->get_domain();
                            if (fault_domain != share_domain)
                            {
                                current_domain = share_domain;
                                fass_update_pds(share_space, cpd_entry, share_sec, fault_section);
                                current_domain = space->get_domain();
                                return;
                            }
                        }
                        is_valid = true;
                    }
                    goto handle_fault;
                }
            }
        }
    }

    if (fass_sync_address(space, fault_addr, &is_valid))
        return;

handle_fault:
    current->arch.misc.fault.fault_addr = fault_addr;
    current->arch.misc.fault.page_fault_continuation = ASM_CONTINUATION;
    return;
}

void fass_update_pds (space_t * space, pgent_t cpg, pgent_t spg, word_t fault_section)
{
    spg.set_domain(current_domain);
    get_cpd()[fault_section] = spg;
}

bool fass_sync_address (space_t * space, addr_t fault_addr, bool * is_valid)
{
    /* Try lookup the mapping */
    pgent_t *pg = space->pgent((word_t)fault_addr >> PAGE_BITS_TOP_LEVEL);
    pgent_t section = *(pg->subtree(space, pgent_t::size_level0)->next(
                space, pgent_t::size_1m,
                ((word_t)fault_addr & (PAGE_SIZE_TOP_LEVEL-1)) >> PAGE_BITS_1M));

    word_t fault_section = (word_t)fault_addr >> PAGE_BITS_1M;
    pgent_t cpg = get_cpd()[fault_section];

    if (EXPECT_TRUE((cpg.get_domain() != current_domain) &&
                    space->is_user_area(fault_addr)))
    {
        fass_update_pds(space, cpg, section, fault_section);
        return true;
    }

    return false;
}
