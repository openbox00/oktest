#ifndef __OKL4__ERRNO_H__
#define __OKL4__ERRNO_H__

#include <okl4/arch/config.h>
#include <okl4/types.h>


/**
 *  The operation was successful.
 */
#define OKL4_OK               (0)

/**
 *  One or more arguments to the function were invalid. This may also occur
 *  when a combination of arguments were incompatible with each other.
 */
#define OKL4_INVALID_ARGUMENT (-2)

/**
 *  A blocking kernel operation was aborted by another thread.
 */
#define OKL4_CANCELLED        (-22)

/**
 *  An argument to the function was out of range for the given object. For
 *  instance, this error code is returned if a caller attempts to allocate a
 *  range of memory from a pool outside the range covered by that pool.
 */
#define OKL4_OUT_OF_RANGE     (-3)

/**
 *  The requested resource is already in use. For instance, this error code is
 *  returned if a caller attempts to allocate a specific range of memory from a
 *  resource pool that has already been allocated.
 */
#define OKL4_IN_USE           (-4)

/**
 *  The given allocator has insufficient resources to perform the requested
 *  allocation. This may be due to the allocator being completely exhausted of
 *  resources or as a result of internal fragmentation of range-based
 *  allocators.
 */
#define OKL4_ALLOC_EXHAUSTED  (-5)

/**
 *  The attempted operation could not be carried out, as it would require the
 *  current thread to block. This error may occur, for instance, if a caller
 *  attempts to perform a non-blocking mutex aquire or non-blocking message
 *  send.
 */
#define OKL4_WOULD_BLOCK        (-17)

/**
 *  Returned when the caller attempts to perform a protection domain operation
 *  on a virtual address that the protection domain has no mapping for. For
 *  instance, this error is returned when okl4_pd_handle_pagefault() is called
 *  on a virtual address not covered by the protection domain.
 */
#define OKL4_UNMAPPED           (-21)

/**
 *  Translate a kernel error code into a libokl4 equivalent error code.
 */
int _okl4_convert_kernel_errno(okl4_word_t errno);

#endif /* !__OKL4__ERRNO_H__ */
