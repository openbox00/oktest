#include <stdio.h>
#include "format.h"
#ifdef __USE_POSIX
#include <pthread.h>
#endif

#include <string.h>

/*
 * All of these functions do not lock the I/O stream.  They all end up calling
 * format which handles the locking. 
 */

int
printf(const char *format, ...)
{

    int ret;
    va_list ap;

    va_start(ap, format);
    ret = vfprintf(stdout, format, ap);
    va_end(ap);

    return ret;
}
