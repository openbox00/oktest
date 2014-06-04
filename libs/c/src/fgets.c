#include <stdio.h>
#include <string.h>
#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include "stream.h"
#include "threadsafety.h"

char *
fgets(char *s, int n, FILE *stream)
{

/* If in POSIX environment, this is a cancellation point */
#ifdef __USE_POSIX
    pthread_testcancel();
#endif

    int i;
    int c = EOF;

    //lock_stream(stream);
    for (i = 0; i < n - 1; i++) {
        c = fgetc(stream);
        if (c == EOF) {
            break;
        }
        s[i] = (char)c;
        if (c == '\n') {
            i++;
            break;
        }
    }
    //unlock_stream(stream);

    s[i] = '\0';
    if (c == EOF && i == 0) {
        return NULL;
    } else {
        return s;
    }
}
