/*
 * Author: Alex Webster
 */
#ifndef __C_H__
#define __C_H__

#if defined(_lint)
#include <compat/toolchain/flint_c.h>
#elif defined(__ADS__)
#include <compat/toolchain/ads_c.h>
#elif defined(__GNUC__)
#include <compat/toolchain/gnu_c.h>
#elif defined(__RVCT_GNU__)
#include <compat/toolchain/rvct_gnu_c.h>
#elif defined(__RVCT__)
#include <compat/toolchain/rvct_c.h>
#else
#error "Unknown compiler"
#endif
#endif
