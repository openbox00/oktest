#include <okl4/types.h>
#include <okl4/env.h>

#ifdef __ADS__
/*
 * For ADS, force the api symbol to be used so that the ADS linker
 * won't remove it from the final link.  This is not a problem for
 * RVCT and gcc.
 */
#pragma import(okl4_api_version)
#endif

okl4_env_segment_t *
okl4_env_get_segment(const char *name)
{
    okl4_env_segments_t *segment_table;
    okl4_word_t *idx;

    segment_table = OKL4_ENV_GET_SEGMENTS("SEGMENTS");
    idx = okl4_env_get(name);

    if (idx == NULL || segment_table == NULL) {
        return NULL;
    }

    return &segment_table->segments[*idx];
}
