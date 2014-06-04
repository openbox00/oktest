#include <stdio.h>
#include <errno.h>

#include "stream.h"
#include "threadsafety.h"

int
fileno(FILE *stream)
{
    int res;

    if (stream == NULL) {
        errno = EBADF;
        return -1;
    }

    //lock_stream(stream);
    res = (int)stream->handle;
    //unlock_stream(stream);
    if (res < 0) {
        errno = EBADF;
    }
    return res;
}
