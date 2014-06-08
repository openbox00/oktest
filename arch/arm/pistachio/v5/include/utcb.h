/*
 * Description:   ARM9 UTCB access
 */
#ifndef __ARM__V5__UTCB_H__
#define __ARM__V5__UTCB_H__

#undef USER_UTCB_REF
#define USER_UTCB_REF           (*(word_t *)(USER_UTCB_PAGE + 0xff0))

#endif /* !__ARM__V5__UTCB_H__ */
