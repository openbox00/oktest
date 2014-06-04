#include <stdio.h>
#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include "stream.h"
#include "threadsafety.h"

size_t
fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{

/* If in POSIX environment, this is a cancellation point */
#ifdef __USE_POSIX
    pthread_testcancel();
#endif

    size_t elems, sz;
    unsigned char *p = ptr;

    //lock_stream(stream);
    for (elems = 0; elems < nmemb; elems++) {
        for (sz = 0; sz < size; sz++, p++) {
            int ch;

            if ((ch = fgetc(stream)) == EOF) {
                goto out;
            }
            *p = (unsigned char)ch;
        }
    }
  out:
    //unlock_stream(stream);
    return elems;
}
