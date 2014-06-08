#include <string.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Fill memory at s with (n) * byte value 'c'
 */

void *
memset(void *s, int c, size_t n)
{
    unsigned char *ss = (unsigned char *)s; 
    while (n--) {
        *ss++ = (unsigned char) c;
    }
    return s;
}

