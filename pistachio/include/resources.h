/*
 * Description:
 */
#ifndef __API__V4__RESOURCES_H__
#define __API__V4__RESOURCES_H__

#include <bitmask.h>


class tcb_t;

/*
 * ThreadControl resource bits encoding
 */
class tc_resources_t
{
public:
    bool is_valid(void) { return bits.valid; }

public:
    union {
        word_t raw;
        struct {
            BITFIELD2(word_t,
                      arch  : BITS_WORD - 1,
                      valid : 1);
        } bits;
    };
};

/*
 * Generic thread resources inherited by threads
 */
class generic_thread_resources_t
{
public:
    void dump(tcb_t * tcb) { }
    void save(tcb_t * tcb) { }
    void load(tcb_t * tcb) { }
    void purge(tcb_t * tcb) { }
    void init(tcb_t * tcb) { }
    void free(tcb_t * tcb) { }
public:
    bool control(tcb_t * tcb, tc_resources_t resources)
        {
            //get_current_tcb()->set_error_code(EINVALID_PARAM);
            //return false;
            return true;
        }
};

#include <arch/resources.h>


#if !defined(HAVE_RESOURCE_TYPE_E)
typedef word_t  resource_bits_t;
#else


/**
 * Abstract class for handling resource bit settings.
 */
class resource_bits_t
{
    bitmask_t   resource_bits;

public:

    /**
     * Intialize resources (i.e., clear all resources).
     */
    inline void init (void)
        { resource_bits = 0; }

    /**
     * Clear all resources.
     */
    inline void clear (void)
        { resource_bits = 0; }

    /**
     * Add resource to resource bits.
     * @param t         type of resource
     * @return new resource bits
     */
    inline resource_bits_t operator += (resource_type_e t)
        {
            resource_bits += (int) t;
            return *this;
        }

    /**
     * Remove resource from resource bits.
     * @param t         type of resource
     * @return new resource bits
     */
    inline resource_bits_t operator -= (resource_type_e t)
        {
            resource_bits -= (int) t;
            return *this;
        }

    /**
     * Check if any resouces are registered.
     * @return true if any resources are registered, false otherwise
     */
    bool have_resources (void)
        {
            return (word_t) resource_bits != 0;
        }

    /**
     * Check if indicated resource is registered.
     * @param t         type of resource
     * @return true if resource is registered, false otherwise
     */
    bool have_resource (resource_type_e t)
        {
            return resource_bits.is_set ((int) t);
        }

    /**
     * Convert resource bits to a word (e.g., for printing).
     * @return the resource mask
     */
    inline operator word_t (void)
        {
            return (word_t) resource_bits;
        }
};

#endif

#endif /* !__API__V4__RESOURCES_H__ */
