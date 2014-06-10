#include <stdio.h>

int
puts(const char *s)
{
    while (*s != '\0')
        (void)fputc(*s++, stdout);
    (void)fputc('\n', stdout);

    return 0;
}
