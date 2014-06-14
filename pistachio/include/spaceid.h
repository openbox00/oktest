#ifndef __L4__SPACEID_H__
#define __L4__SPACEID_H__


class spaceid_t
{
public:
    word_t get_spaceno() const { return this->spaceno; }

private:
    union {
        word_t raw;
        word_t spaceno;
    };
};


#endif /*__L4__SPACEID_H__*/
