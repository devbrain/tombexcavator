#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <iostream>

#if defined(__BORLANDC__)
    typedef unsigned long off_t;
#endif
// ===========================================================================
namespace types
{
    typedef unsigned char  byte;
    typedef unsigned int   dword;
    typedef unsigned short word;

    union reg16_t
    {
            byte component [2];
            word data;


            reg16_t operator = (word w)
            {
                data = w;
                return *this;
            }
    };

    union reg32_t
    {
        reg16_t component [2];
        dword   data;

        reg32_t operator = (dword w)
        {
            data = w;
            return *this;
        }
    };
}
// ---------------------------------------------------------------------------------------
#define R_LOW(X)  X.component [0]
#define R_HIGH(X) X.component [1]
// ---------------------------------------------------------------------------------------
inline
std::ostream& operator << (std::ostream& os, types::reg16_t r)
{
    os << r.data;
    return os;
}
// ---------------------------------------------------------------------------------------
inline
short operator | (const types::reg16_t& a, const types::reg16_t& b)
{
    return (a.data | b.data);
}
// ---------------------------------------------------------------------------------------

#endif // TYPES_H_INCLUDED
