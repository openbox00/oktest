#include <okl4/types.h>
#include <okl4/errno.h>

#include <stdio.h>

int
_okl4_convert_kernel_errno(okl4_word_t errno)
{
    switch(errno) {
        case L4_ErrOk:
            return OKL4_OK;
        case L4_ErrInvalidThread:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrInvalidSpace:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrInvalidScheduler:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrInvalidParam:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrUtcbArea:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrNoMem:
            return OKL4_ALLOC_EXHAUSTED;
        case L4_ErrInvalidMutex:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrDomainConflict:
            return OKL4_INVALID_ARGUMENT;
        case L4_ErrMutexBusy:
            return OKL4_WOULD_BLOCK;
        default:
            for (;;) {
            }
    }
}

