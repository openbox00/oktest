/**
 * @file
 * @brief The configuration information from the kerenel needed by SoC code.
 */
#ifndef __KERNEL_CONFIG_H__
#define __KERNEL_CONFIG_H__

/* default system is uniprocessor - override in CFLAGS if neccesary */
#ifndef CONFIG_NUM_DOMAINS
#define CONFIG_NUM_DOMAINS 1
#endif

#ifndef CONFIG_NUM_UNITS
#define CONFIG_NUM_UNITS 1
#endif

#endif /* !__KERNEL_CONFIG_H__ */
