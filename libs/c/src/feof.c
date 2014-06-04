#include <stdio.h>

#include "stream.h"
#include "threadsafety.h"

int
feof(FILE *f)
{
    int res;

  //  lock_stream(f);
    res = f->eof;
  //  unlock_stream(f);
    return res;
}
