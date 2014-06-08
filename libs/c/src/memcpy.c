#include <string.h>
#include <stdint.h>

/* copy n bytes from s to d; the regions must not overlap */
/* THREAD SAFE */

void *
memcpy(void *d, const void *s, size_t n)
{
    unsigned char *dd = (unsigned char *)d;
    const unsigned char *ss = (unsigned char *)s;
    while (n--) {
        *dd++ = *ss++;
    }
    return d;
}

