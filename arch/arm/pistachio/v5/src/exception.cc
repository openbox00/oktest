/*
 * Description:   ARMv5 syscall and page fault handlers
 */
#include <space.h>
#include <tcb.h>

/* ARM program counter fixup - LSB used for context frame type */
#define PC(x)           (x & (~1UL))

bool fass_sync_address (space_t * space, addr_t fault_addr, bool * is_valid = NULL);
void fass_update_pds (space_t * space, pgent_t cpg, pgent_t spg, word_t fault_section);

extern "C" void arm_memory_abort(word_t fault_status, addr_t fault_addr,
        arm_irq_context_t *context, word_t data_abt)
{
    /* Note, fs = 0 for execute fault */
    word_t fs = fault_status & 0xf;
    tcb_t * current = get_current_tcb();
    if (data_abt)
    {
        switch(fs) {
            /* Allowed abort exception types */
            case 7: case 5: case 15: case 13: case 9: case 11:
                break;
            case 2:     /* Terminal exception, not recoverable */
            default:    /* Unhandled exception */
            {
                bool is_kernel = ((context->cpsr & CPSR_MODE_MASK) != CPSR_USER_MODE);

                if (is_kernel) {
                }
                current->arch.misc.exception.exception_continuation = ASM_CONTINUATION;
                current->arch.misc.exception.exception_context = context;
            }
        }
    }

    space_t *space = current->get_space();

    if (EXPECT_FALSE(space == NULL))
    {
        space = get_kernel_space();
    }
    else
    {
        /* PID relocation */
        if ((word_t)fault_addr < PID_AREA_SIZE)
        {
            word_t pid = space->get_pid();
            fault_addr = (addr_t)((word_t)fault_addr + (pid << 25));
        }
    }

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
            /* if a domain fault */
            if (!(space->get_domain_mask() & (3UL << (fault_domain*2))))
            {
                space_t *share_space = get_arm_fass()->get_space(fault_domain);

                if (share_space && share_space->is_client_space(space))
                {
                    bool manager = space->is_manager_of_domain(share_space);
                    space->add_domain_access(fault_domain, manager);
                    current_domain_mask = space->get_domain_mask();
                    domain_dirty |= current_domain_mask;
                    return;
                }
            }

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
                            bool manager = space->is_manager_of_domain(share_space);

                            if (share_domain == INVALID_DOMAIN)
                            {
                                get_arm_fass()->activate_other_domain(share_space);

                                /* get domain again if we activate_other_domain */
                                share_domain = share_space->get_domain();

                                space->add_domain_access(share_domain, manager);

                                current_domain_mask = space->get_domain_mask();
                                domain_dirty |= current_domain_mask;

                                /* Update the faulter domain's set of sections in the CPD */
                                get_arm_fass()->add_set(share_domain, fault_section);
                                /* Update the CPD */

                                share_sec.set_domain(share_domain);

                                /* keep TLB in sync if replacing entries */
                                if (get_cpd()[fault_section].is_valid(space, pgent_t::size_1m)) {
                                    arm_cache::tlb_flush();
                                }
                                get_cpd()[fault_section] = share_sec;
                                return;
                            }
                            if (fault_domain != share_domain)
                            {
                                space->add_domain_access(share_domain, manager);
                                current_domain_mask = space->get_domain_mask();
                                domain_dirty |= current_domain_mask;

                                current_domain = share_domain;
                                fass_update_pds(share_space, cpd_entry, share_sec, fault_section);
                                current_domain = space->get_domain();
                                return;
                            }
                        }
                        is_valid = true;
                    }

                    /* translation fault in shared area */
                    if (section.is_callback()) {
                        current->set_preempted_ip( current->get_user_ip() );
                        current->get_utcb()->share_fault_addr = (word_t)fault_addr;
                        current->set_user_ip( current->get_preempt_callback_ip() );
                        return;
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
    bool cpd_section_valid = cpg.is_valid(get_kernel_space(), pgent_t::size_1m);
    arm_domain_t cpd_section_domain = cpg.get_domain();
    /* Clean the kernel's UTCB synonym if necessary */
    bool need_flush = (cpd_section_valid &&
            TEST_DOMAIN(domain_dirty, cpd_section_domain));

    /* If there is an existing mapping in the CPD for this section,
     * remove it from the owner domain's set of sections in the CPD
     */
    if (cpd_section_valid)
        get_arm_fass()->remove_set(cpd_section_domain, fault_section);

    /* Update the faulter domain's set of sections in the CPD */
    get_arm_fass()->add_set(current_domain, fault_section);

    /* Update the CPD */
    spg.set_domain(current_domain);
    get_cpd()[fault_section] = spg;

    if (need_flush) {
        bool flush = 1;

        space_t *old_space = get_arm_fass()->get_space(cpd_section_domain);
        if (old_space) {
            word_t old_vspace = old_space->get_vspace();

            if (old_vspace && (space->get_vspace() == old_vspace))
                flush = 0;
        }

        get_arm_fass()->clean_all(flush);
        domain_dirty |= current_domain_mask;
    } else {
        arm_cache::tlb_flush();
    }
}
bool fass_sync_address (space_t * space, addr_t fault_addr, bool * is_valid)
{
    /* Try lookup the mapping */
    pgent_t *pg = space->pgent((word_t)fault_addr >> PAGE_BITS_TOP_LEVEL);
    if (EXPECT_FALSE(!pg->is_valid(space, pgent_t::size_level0)))
        return false;
    pgent_t section = *(pg->subtree(space, pgent_t::size_level0)->next(
                space, pgent_t::size_1m,
                ((word_t)fault_addr & (PAGE_SIZE_TOP_LEVEL-1)) >> PAGE_BITS_1M));
    if (EXPECT_FALSE(!section.is_valid(space, pgent_t::size_1m)))
        return false;
    if (section.is_subtree(space, pgent_t::size_1m))
    {
        pgent_t& leaf =
            *section.subtree(space, pgent_t::size_1m)->next(
                    space, pgent_t::size_min,
                    ((word_t)fault_addr & (PAGE_SIZE_1M-1)) >> ARM_PAGE_BITS);
        if (leaf.l2.fault.zero == 0)
            return false;
    }

    word_t fault_section = (word_t)fault_addr >> PAGE_BITS_1M;
    pgent_t cpg = get_cpd()[fault_section];

    if (EXPECT_TRUE((cpg.get_domain() != current_domain) &&
                    space->is_user_area(fault_addr)))
    {
        fass_update_pds(space, cpg, section, fault_section);
        return true;
    }
    /* this is the uncommon case so take the performance hit (dereferencing the pointer) here */
    if (is_valid)
        *is_valid = true;
    return false;
}
