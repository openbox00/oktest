/*
 * Description:   Multi Processor support
 */
#ifndef __GENERIC__MP_H__
#define __GENERIC__MP_H__

#include <types.h>
#include <debug.h>

/*
 * This class is:
 *  - an interface description
 *  - a porting helper, the real mp_t can be derived from it
 *
 *   NOTE: If we are not building an MP kernel, then we instansiate a empty version, with
 *   no over-ridden members that will give us uniprocessor support by default
 */


static int generic_execution_units[1] = {1};

class generic_mp_t
{
public:

    /* Does what ever is necessary to find the amount of packages/cores/hwthreads. */
    void probe() {
        execution_units = generic_execution_units;
    }

    /* These may be hardcoded for a particular platform or include specific
     * code to find the corerct values */
    inline unsigned int get_num_scheduler_domains()      { return 1; }

    inline void startup_context(cpu_context_t context)  { }
    inline void stop_context(cpu_context_t context)     { }
    inline void interrupt_context(cpu_context_t c) { UNIMPLEMENTED();}

protected:
    int * execution_units;
};


#if defined(CONFIG_MDOMAINS) || defined(CONFIG_MUNITS)
/* If we are a multi processing system of any variety, include the platform specific version to override the above
 * implementation
 *
 */
#include <arch/mp.h>

#define ON_CONFIG_MDOMAINS(x) do { x; } while(0)
#define ON_CONFIG_MUNITS(x) do { x; } while(0)

#else

/* Handle the single processor case */
inline cpu_context_t get_current_context(){
        return 0;
}

class mp_t : public generic_mp_t {};

#define ON_CONFIG_MDOMAINS(x)
#define ON_CONFIG_MUNITS(x)

#endif

inline mp_t* get_mp()
{
    extern mp_t mp;
    return &mp;
}

#endif /* !__GENERIC__MP_H__ */
