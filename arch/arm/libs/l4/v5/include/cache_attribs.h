#ifndef __L4__ARM__V5__CACHE_ATTRIBS_H__
#define __L4__ARM__V5__CACHE_ATTRIBS_H__

#include <l4/cpu/cache_attribs.h>

#ifdef __USE_DEFAULT_V5_CACHE_ATTRIBUTES
#undef __USE_DEFAULT_V5_CACHE_ATTRIBUTES

#define CACHE_ATTRIB_UNCACHED       0
#define CACHE_ATTRIB_WRITECOMBINE   1
#define CACHE_ATTRIB_WRITETHROUGH   2
#define CACHE_ATTRIB_WRITEBACK      3

#define L4_DefaultMemory            CACHE_ATTRIB_WRITEBACK
#define L4_CachedMemory             CACHE_ATTRIB_WRITEBACK
#define L4_UncachedMemory           CACHE_ATTRIB_UNCACHED
#define L4_WriteBackMemory          CACHE_ATTRIB_WRITEBACK
#define L4_WriteThroughMemory       CACHE_ATTRIB_WRITETHROUGH
#define L4_CohearentMemory          CACHE_ATTRIB_UNCACHED
#define L4_IOMemory                 CACHE_ATTRIB_UNCACHED
#define L4_IOCombinedMemory         CACHE_ATTRIB_UNCACHED
#define L4_StrongOrderedMemory      CACHE_ATTRIB_UNCACHED

#endif

#endif /* !__L4_ARM__V5__CACHE_ATTRIBS_H__ */
