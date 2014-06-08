#include <stdio.h>
#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include "stream.h"
#include "threadsafety.h"

int
puts(const char *s)
{

/* If in POSIX environment, this is a cancellation point */
#ifdef __USE_POSIX
    pthread_testcancel();
#endif

    //lock_stream(stdout);
    while (*s != '\0')
        (void)fputc(*s++, stdout);
    (void)fputc('\n', stdout);
    //unlock_stream(stdout);
    return 0;
}
