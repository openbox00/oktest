#ifndef __ASM_H__
#define __ASM_H__

#if defined(_lint)
#include <compat/toolchain/flint_asm.h>
#elif defined(__ADS__)
#include <compat/toolchain/ads_asm.h>
#elif defined(__GNUC__)
#include <compat/toolchain/gnu_asm.h>
#elif defined(__RVCT_GNU__)
#include <compat/toolchain/rvct_gnu_asm.h>
#elif defined(__RVCT__)
#include <compat/toolchain/rvct_asm.h>
#else
#error "Unknown compiler"
#endif

#include <compat/arch/asm.h>

#endif
