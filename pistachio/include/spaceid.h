/*
 * Description:
 */
#ifndef __L4__SPACEID_H__
#define __L4__SPACEID_H__

#include <idtable.h>

class space_t;
class kmem_resource_t;

class spaceid_t
{
public:
    static spaceid_t kernelspace()
        {
            spaceid_t sid;
            sid.raw = ~0UL;
            return sid;
        }

    static spaceid_t nilspace()
        {
            spaceid_t sid;
            sid.raw = TWOSCOMP(1UL);
            return sid;
        }

    static spaceid_t rootspace()
        {
            spaceid_t sid;
            sid.raw = 0;
            return sid;
        }

    static spaceid_t spaceid(word_t spaceno_)
        {
            spaceid_t sid;
            sid.spaceno = spaceno_;
            return sid;
        }

    /* check for specific (well known) space ids */
    bool is_nilspace() { return this->raw == TWOSCOMP(1UL); }
    bool is_rootspace() { return this->raw == 0; }

    word_t get_spaceno() const { return this->spaceno; }

    word_t get_raw() { return this->raw; }
    void set_raw(word_t raw_) { this->raw = raw_; }

    /* operators */
    bool operator == (const spaceid_t & sid) const
        {
            return this->raw == sid.raw;
        }

    bool operator != (const spaceid_t & sid) const
        {
            return this->raw != sid.raw;
        }

private:
    union {
        word_t raw;
        word_t spaceno;
    };
};

class spaceid_lookup_t : private id_lookup_t
{
public:
    bool is_valid(spaceid_t id) {
        return valid_id(id.get_spaceno());
    }

    space_t * lookup_space(spaceid_t id) {
        return (space_t*)lookup_id(id.get_spaceno());
    }

public:
    void add_space(spaceid_t id, space_t* space) {
        add_object(id.get_spaceno(), space);
    }

    void remove_space(spaceid_t id) {
        remove_object(id.get_spaceno());
    }
private:
    friend void init_spaceids(word_t max_spaceids, kmem_resource_t *kresource);
};

INLINE spaceid_lookup_t *
get_space_list()
{
    extern spaceid_lookup_t space_lookup;
    return &space_lookup;
}

void init_spaceids(word_t max_spaceids, kmem_resource_t *kresource);

#endif /*__L4__SPACEID_H__*/
