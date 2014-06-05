#include <stdlib.h>

void
exit(int status)
{
    /* Call registered atexit() functions */

    /* Flush unbuffered data */

    /* Close all streams */

    /* Remove anything created by tmpfile() */

    /* Call _Exit() */
    _Exit(status);
}
