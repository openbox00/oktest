/*
 * Description:   Prioritized constructors support
 */
#ifndef __CTORS_H__
#define __CTORS_H__

/**
 * adds initialization priority to a static object
 * @param prio_class    major initialization group
 * @param prio          group-local priority, higher numbers
 *                      indicate later initialization
 *
 * forces error message if @c prio is in next group
 *
 * example:     foo_t foo CTORPRIO(CTORPRIO_CPU, 100);
 *
 */
#define CTORPRIO(prio_class, prio) \
  __attribute__((init_priority(prio >= 10000 ? 0 : 65535-(prio_class+prio))))


// Major priority groups for static constructors
#define CTORPRIO_CPU    30000
#define CTORPRIO_NODE   20000
#define CTORPRIO_GLOBAL 10000


// prototypes
void call_cpu_ctors();
void call_node_ctors();
void call_global_ctors();

#endif /* !__CTORS_H__ */
