/**
 * @file Generic identifier lookup table.
 */
#ifndef __IDTABLE_H__
#define __IDTABLE_H__

#include <kernel/macros.h>

class id_lookup_t
{
public:
    bool valid_id(word_t id)
        {
            return (id > max_id) ? false : true;
        }

    addr_t lookup_id(word_t id)
        {
            if (!valid_id(id)) {
                return NULL;
            }
            return id_table[id];
        }
public:
    void add_object(word_t id, addr_t obj)
        {
            id_table[id] = obj;
        }
    void remove_object(word_t id)
        {
            id_table[id] = NULL;
        }
protected:
    inline void init(addr_t table, word_t entries)
        {
            id_table = (addr_t*)table;
            max_id = entries-1;
        }
private:
    addr_t* id_table;
    word_t max_id;
};

#endif /*!__IDTABLE_H__*/
