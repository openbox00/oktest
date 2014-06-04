

#include <l4.h>
#define BUILD_TCB_LAYOUT
#define O(w,x) (u32_t)((char*) &w.x - (char*)(&w))
#if defined(__GNUC__)
#define DEFINE(label, val)     __asm__ __volatile__ ("DEFINE " #label " %n0" : : "i" (-val));
#elif defined(__RVCT_GNU__)
#define DEFINE(label, val)     DEFINE label __cpp(val);
#endif
#include </home/asic/okl4_3.0/arch/arm/pistachio/include/ktcb.h>
arch_ktcb_t foo;


#if defined(__RVCT_GNU__)
__asm
#endif
void make_offsets() {
DEFINE(OFS_ARCH_KTCB_CONTEXT, O(foo, context)) 
DEFINE(OFS_ARCH_KTCB_MISC, O(foo, misc)) 
DEFINE(OFS_ARCH_KTCB_EXC_NUM, O(foo, exc_num)) 

}
