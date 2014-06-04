#include <stdio.h>
#include <stdlib.h>

#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include "stream.h"
#include "threadsafety.h"

int
fclose(FILE *stream)
{
    int r;

#ifdef __USE_POSIX
    /* If in POSIX environment, this is a cancellation point */
    pthread_testcancel();
#endif

    fflush(stream);
    //lock_stream(stream);
    r = stream->close_fn(stream->handle);
   // unlock_stream(stream);
#if 0
#ifdef THREAD_SAFE
    OKL4_MUTEX_FREE(&stream->mutex);
#endif
#endif
    return r;
}
