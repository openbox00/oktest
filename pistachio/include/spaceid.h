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
    word_t get_spaceno() const { return this->spaceno; }

private:
    union {
        word_t raw;
        word_t spaceno;
    };
};

class spaceid_lookup_t : private id_lookup_t
{
public:
    space_t * lookup_space(spaceid_t id) {
        return (space_t*)lookup_id(id.get_spaceno());
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
