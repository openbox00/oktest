#include <stdio.h>
#ifdef __USE_POSIX
#include <pthread.h>
#endif
#include <errno.h>

#include "stream.h"
#include "threadsafety.h"

int
fgetc(FILE *stream)
{

/* If in POSIX environment, this is a cancellation point */
#ifdef __USE_POSIX
    pthread_testcancel();
#endif

    unsigned char ch;
    int r;

    //lock_stream(stream);
    if (stream->unget_pos) {
        ch = stream->unget_stack[--stream->unget_pos];
        //unlock_stream(stream);
        return (int)ch;
    }

    /* This is where we should do input buffering */
    r = stream->read_fn(&ch, stream->current_pos, 1, stream->handle);
    if (r == 1) {
        /* Success */
        stream->current_pos++;
       // unlock_stream(stream);
        return (int)ch;
    } 
    else if (r == 0) {
        stream->eof = 1;
      //  unlock_stream(stream);
        return EOF;
    } else {
        stream->eof = 1;
        stream->error = errno;
       // unlock_stream(stream);
        return EOF;
    }
}
