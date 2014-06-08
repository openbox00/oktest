#include <stdio.h>

#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include <string.h>

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
        *stream->buffer_end++ = ch;
        stream->current_pos++;
        if ((stream->buffering_mode == _IOLBF && ch == '\n') ||
            (stream->buffer_end - stream->buffer >= stream->buffer_size - 1)) {
            fflush(stream);
        }

    } else {
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
            //stream->error = errno;
        }
    }

    return c;
}
