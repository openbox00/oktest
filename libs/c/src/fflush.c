#include <stdio.h>
#ifdef __USE_POSIX
#include <pthread.h>
#endif
#include "stream.h"
#include "threadsafety.h"

int
fflush(FILE *file)
{

/* If in POSIX environment, this is a cancellation point */
#ifdef __USE_POSIX
    pthread_testcancel();
#endif

    /* TODO: set file->error to errno on error */

    int ret = EOF;

    /* Ensure the file is actually buffered */
    if (file->buffering_mode != _IOLBF && file->buffering_mode != _IOFBF) {
        return ret;
    }

    /* Write and reset the buffer */
    int count = file->buffer_end - file->buffer;
    int written = file->write_fn(file->buffer, file->current_pos, count, file->handle);
    file->buffer_end = file->buffer;

    /* Call the flush method on the file.
     * This is a file-specific method to clear any underlying stream etc. */
    if (file->flush_fn) {
        file->flush_fn(written);
    }

    /* Success if all written */
    if (written == count) {
        ret = 0;
    } else {
        //file->error = errno;
    }

    return ret;
}
