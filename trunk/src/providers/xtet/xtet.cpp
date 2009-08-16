#include "xtet.hpp"


xtet_c::xtet_c ()
{
    m_ifs.open ("Xtet._x_", std::ios::binary | std::ios::in);
    m_header = new header_c (m_ifs);

    dword_24h    = 0x80000000;
    m_unk2       = (m_header->unk2 () & 0x0000FFFF);
    m_chunks_num = m_header->u0_chunks ();

    m_u0 = new u0_c (m_ifs, *m_header);

}
// -----------------------------------------------------------------------------
xtet_c::~xtet_c ()
{
    delete m_header;
    delete m_u0;
}
// -----------------------------------------------------------------------------
bool xtet_c::check_check_sum () const
{
    return m_header->eval_check_sum () == m_header->check_sum ();
}
// -----------------------------------------------------------------------------
std::ostream& operator << (std::ostream& os, const xtet_c& xtet)
{
    os << *xtet.m_header;
    return os;
}
