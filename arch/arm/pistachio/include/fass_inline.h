INLINE void arm_fass_t::activate_other_domain(space_t *space)
{
    word_t section = ( UTCB_AREA_START/ARM_SECTION_SIZE + (space->get_space_id().get_spaceno()) );
    pgent_t entry = *space->pgent_utcb();
    get_cpd()[section] = entry;
}

