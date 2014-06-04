#include <stdio.h>

#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include <string.h>
#include <errno.h>
#include <assert.h>

#include "stream.h"
#include "threadsafety.h"

int
fputc(int c, FILE *stream)
{

/* If in POSIX environment, this is a cancellation point */
#ifdef __USE_POSIX
    pthread_testcancel();
#endif

    unsigned char ch = (unsigned char)c;
    int r;

    /* If file is buffered, add to buffer */
    if (stream->buffering_mode != _IONBF) {
        //lock_stream(stream);
        *stream->buffer_end++ = ch;
        stream->current_pos++;
        //unlock_stream(stream);

        /* If the buffer is full, or it is line buffered and this is a new line, flush
           We can't be locked when we make this call */
        if ((stream->buffering_mode == _IOLBF && ch == '\n') ||
            (stream->buffer_end - stream->buffer >= stream->buffer_size - 1)) {
            fflush(stream);
        }

    } else {
        //lock_stream(stream);
        assert(stream->write_fn != NULL);
        r = stream->write_fn(&ch, stream->current_pos, 1, stream->handle);
        if (r == 1) {
            stream->current_pos++; /* Success */
        } 
        else if (r == 0) {
            c = EOF;
            stream->eof = 1;
        } else {
            c = EOF;
            stream->eof = 1;
            stream->error = errno;
        }
        //unlock_stream(stream);
    }

    return c;
}
