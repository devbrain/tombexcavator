//
// Created by igor on 04/04/2021.
//

#include <iostream>
#include <tomb-excavator/formats/iff/fourcc.hh>

namespace formats::iff
{
#define ID_OP(S) return (a.m_id S b.m_id)
    // --------------------------------------------------------------
    bool operator == (const chunk_type& a, const chunk_type& b)
    {
        ID_OP (==);
    }
    // --------------------------------------------------------------
    bool operator != (const chunk_type& a, const chunk_type& b)
    {
        ID_OP (!=);
    }
    // --------------------------------------------------------------
    bool operator <  (const chunk_type& a, const chunk_type& b)
    {
        ID_OP (<);
    }
    // --------------------------------------------------------------
    bool operator <= (const chunk_type& a, const chunk_type& b)
    {
        ID_OP (<=);
    }
    // --------------------------------------------------------------
    bool operator >  (const chunk_type& a, const chunk_type& b)
    {
        ID_OP (>);
    }
    // --------------------------------------------------------------
    bool operator >= (const chunk_type& a, const chunk_type& b)
    {
        ID_OP (>=);
    }
    // ---------------------------------------------------------------
    std::ostream& operator << (std::ostream& os, const chunk_type& nm)
    {
        os << nm.to_string();
        return os;
    }
}
