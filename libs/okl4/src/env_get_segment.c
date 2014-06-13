#include <okl4/types.h>
#include <okl4/env.h>

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
