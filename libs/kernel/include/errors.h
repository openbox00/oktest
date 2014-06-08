/**
 * @file
 * @brief The standard OKL4 error numbers.
 */
#ifndef __ERRORS_H__
#define __ERRORS_H__

/*
 * Error code values
 */

#define EOK                     (0)
/* ENO_PRIVILEGE was 1 */
#define ERES_1                  (1)
#define EINVALID_THREAD         (2)
#define EINVALID_SPACE          (3)
#define EINVALID_SCHEDULER      (4)
#define EINVALID_PARAM          (5)
#define EUTCB_AREA              (6)
/* EKIP_AREA was 7 */
#define ERES_2                  (7)
#define ENO_MEM                 (8)
#define ESPACE_NOT_EMPTY        (9)
#define ECLIST_NOT_EMPTY        (9)
#define EINVALID_MUTEX          (10)
#define EMUTEX_BUSY             (11)
#define EINVALID_CLIST          (12)
#define EINVALID_CAP            (13)
#define EDOMAIN_CONFLICT        (15)
#define ENOT_IMPLEMENTED        (16)

#endif /* ! __ERRORS_H__ */
