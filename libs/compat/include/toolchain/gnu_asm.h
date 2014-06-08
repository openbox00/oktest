#ifndef __GNU_ASM_H__
#define __GNU_ASM_H__

#define EXPORT                  .global
#define LABEL(name)             name:
#define END

#define BEGIN_PROC(name)                        \
    .global name;                               \
    .align;                                     \
name:

#define END_PROC(name)                          \
    ;

#endif
