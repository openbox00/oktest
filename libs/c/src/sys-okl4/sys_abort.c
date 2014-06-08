#include <stdlib.h>

void
abort(void)
{
#if !defined(NANOKERNEL)
    //L4_KDB_Enter("L4_Rootserver abort()ed");
#endif
    while (1);                  /* We don't return after this */
}
