#ifndef __OKL4__ENV_H__
#define __OKL4__ENV_H__

#include <okl4/types.h>

extern okl4_env_t *__okl4_environ;

/**
 *  @file
 *
 *  The env.h Header.
 *
 *  The env.h header provides the functionality required for obtaining
 *  predefined, pre-constructed and pre-initialized objects from Elfweaver as well
 *  as general metadata about the execution environment. The collection of
 *  objects and metadata obtained in this manner are referred to as the @a
 *  OKL4 object environment.
 *
 *  The OKL4 object environment behaves in a similar manner to a @a dictionary.  It is
 *  queried by providing strings as lookup @a keys.  The returned value is
 *  of arbitary type and must be cast appropriately prior to interpretation.
 *
 */

/** Magic header value for the OKL4 environment structure. */
#define OKL4_ENV_HEADER_MAGIC  0xb2a4UL

/**
 *  The okl4_envitem structure is used to represent a single object in
 *  the OKL4 object environment. It should be noted that this structure 
 *  is internal to the implementation of the OKL4 object environment.
 *
 */
struct okl4_envitem {
    char *name;
    void *item;
};

/**
 *  The okl4_env structure is used to represent the entire OKL4
 *  object environment. It should be noted that this structure 
 *  is internal to the implementation of the OKL4 object environment.
 *
 */
struct okl4_env {
    okl4_u16_t len;
    okl4_u16_t id;

     /* Variable sized Array of environment items. */
    okl4_envitem_t env_item[1];
};

/**
 *  The okl4_env_get() function is used to lookup an environment entry with
 *  the lookup key specified by the @a name argument. Note that @a name 
 *  should be specified in uppercase.
 *
 *  This function returns a pointer to the environment item or NULL if no
 *  environment item exists with the given @a name.
 *
 */
void *okl4_env_get(const char *name);

int _strcasecmp(const char *s1, const char *s2);

#include "env_types.h"

/**
 *  
 */
okl4_env_segment_t *okl4_env_get_segment(const char *name);

/**
 *  The okl4_env_get_args() function is used to retrieve the okl4_env_args_t
 *  structure in the environment, which is used to stroe command line arguments
 *  provided to the cell.
 *
 *  @param name
 *      The name of the environment variable.  This is typically
 *      "MAIN_ARGS".
 *
 *  @retval NULL
 *      This environment variable does not exist.
 *
 *  @retval others
 *      The pointer to a okl4_env_args_t structure.
 *
 */
okl4_env_args_t *okl4_env_get_args(const char *name);

/**
 *  Given a virtual address of the root server, return details about
 *  the physical memory associated with it.
 *
 *  This function requires the following arguments:
 *
 *  @param vaddr
 *      The virtual address to query.
 *
 *  @param phys
 *      The physical memory item associated with the virtual address.
 *
 *  @param offset
 *      The offset at which the virtal address is located in the 
 *      physical memory item.
 *
 *  @param attributes
 *      The memory attributes associated with the physical
 *      memory.
 *
 *  @param perms
 *      The memory permissions associated with the physical
 *      memory.
 *
 */
int okl4_env_get_physseg_page(okl4_word_t vaddr, okl4_physmem_item_t *phys,
        okl4_word_t *offset, okl4_word_t *attributes, okl4_word_t *perms);


#endif /* !__OKL4__ENV_H__ */
