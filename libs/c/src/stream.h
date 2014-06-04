#ifndef _STREAM_H_
#define _STREAM_H_

#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>
#include <assert.h>
#include <sys/types.h>

#include "threadsafety.h"

#define __UNGET_SIZE 10

struct __file {
    void *handle;

    size_t (*read_fn)(void *, long int, size_t, void *);
    size_t (*write_fn)(const void *, long int, size_t, void *);
    int (*close_fn)(void *);
    long int (*eof_fn)(void *);

    unsigned char buffering_mode;
    char *buffer;

    unsigned char unget_pos;
    long int current_pos;
#if 0
#ifdef THREAD_SAFE
    int mutex_initialised;
    OKL4_MUTEX_TYPE mutex;
#endif
#endif
    int eof;
    int error;

    char unget_stack[__UNGET_SIZE];

    /* Added to end so as not to break existing init code */
    char *buffer_end;
    int buffer_size;
    void (*flush_fn)(int count);
};

#endif /* !_STREAM_H_ */
