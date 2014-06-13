#ifndef __OKL4__ENV_H__
#define __OKL4__ENV_H__

#include <okl4/types.h>

extern okl4_env_t *__okl4_environ;

struct okl4_envitem {
    char *name;
    void *item;
};

struct okl4_env {
    okl4_u16_t len;
    okl4_u16_t id;

     /* Variable sized Array of environment items. */
    okl4_envitem_t env_item[1];
};

void *okl4_env_get(const char *name);

#include "env_types.h"

okl4_env_segment_t *okl4_env_get_segment(const char *name);


#endif /* !__OKL4__ENV_H__ */
