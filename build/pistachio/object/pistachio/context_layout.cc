

#include <l4.h>
#define BUILD_TCB_LAYOUT
#define O(w,x) (u32_t)((char*) &w.x - (char*)(&w))
#if defined(__GNUC__)
#define DEFINE(label, val)     __asm__ __volatile__ ("DEFINE " #label " %n0" : : "i" (-val));
#elif defined(__RVCT_GNU__)
#define DEFINE(label, val)     DEFINE label __cpp(val);
#endif
#include </home/asic/okl4_3.0/arch/arm/pistachio/include/thread.h>
arm_irq_context_t foo;


#if defined(__RVCT_GNU__)
__asm
#endif
void make_offsets() {
DEFINE(PT_KLR, O(foo, klr)) 
DEFINE(PT_R0, O(foo, r0)) 
DEFINE(PT_R1, O(foo, r1)) 
DEFINE(PT_R2, O(foo, r2)) 
DEFINE(PT_R3, O(foo, r3)) 
DEFINE(PT_R4, O(foo, r4)) 
DEFINE(PT_R5, O(foo, r5)) 
DEFINE(PT_R6, O(foo, r6)) 
DEFINE(PT_R7, O(foo, r7)) 
DEFINE(PT_R8, O(foo, r8)) 
DEFINE(PT_R9, O(foo, r9)) 
DEFINE(PT_R10, O(foo, r10)) 
DEFINE(PT_R11, O(foo, r11)) 
DEFINE(PT_R12, O(foo, r12)) 
DEFINE(PT_SP, O(foo, sp)) 
DEFINE(PT_LR, O(foo, lr)) 
DEFINE(PT_PC, O(foo, pc)) 
DEFINE(PT_CPSR, O(foo, cpsr)) 
DEFINE(PT_SIZE, sizeof(arm_irq_context_t))

}
