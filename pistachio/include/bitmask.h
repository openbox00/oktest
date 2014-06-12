/*
 * Description:   Generic bitmask class
 */
#ifndef __BITMASK_H__
#define __BITMASK_H__


/**
 * Generic bitmask manipulation.
 */
class bitmask_t
{
    word_t      maskvalue;

public:

    // Constructors

    inline bitmask_t (void) { maskvalue = 0; }
    inline bitmask_t (word_t initvalue) { maskvalue = initvalue; }

    // Modification

    inline bitmask_t operator + (int n)
        {
            bitmask_t m (maskvalue | (1UL << n));
            return m;
        }

    inline bitmask_t operator - (int n)
        {
            bitmask_t m (maskvalue & ~(1UL << n));
            return m;
        }

    inline bitmask_t operator += (int n)
        {
            maskvalue = maskvalue | (1UL << n);
            return (bitmask_t) maskvalue;
        }

    inline bitmask_t operator -= (int n)
        {
            maskvalue = maskvalue & ~(1UL << n);
            return (bitmask_t) maskvalue;
        }

    // Predicates

    inline bool is_set (int n)
        {
            return (maskvalue & (1UL << n)) != 0;
        }
};



#endif /* !__BITMASK_H__ */
