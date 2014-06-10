#include <stdio.h>
#include "format.h"
#include <string.h>


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
