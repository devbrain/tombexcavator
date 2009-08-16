#include "memarea.h"
#include <algorithm>
#include <string.h>
#include <iomanip>
#include <fstream>
#include <string>
// -----------------------------------------------------------------
memarea_c::memarea_c ()
:   m_low (0xFFFF),
    m_high (0)
{
    memset (m_area, 0, 0xFFFF);
}
// -----------------------------------------------------------------
memarea_c::memarea_c (const char* dumpfile)
:   m_low (0xFFFF),
    m_high (0)
{
    memset (m_area, 0, 0xFFFF);
    std::ifstream ifs (dumpfile);
    ifs.setf( std::ios::hex, std::ios::basefield );
    while (ifs.good ())
    {
        types::word seg, offs;

        ifs >> seg;
        if (!ifs.good ())
        {
            break;
        }
        char sep = ifs.get ();
        if (!ifs.good ())
        {
            break;
        }
        ifs >>  offs;


        for (int i=0; i<16; i++)
        {
            types::word v;
            ifs >> v;
            set_byte (offs + i, (types::byte) v);
        }
    }
}
// -----------------------------------------------------------------
void memarea_c::dump (const char* dumpfile, types::word seg, types::word offset, types::word len)
{
    std::ofstream ofs (dumpfile);
    ofs.setf( std::ios::hex, std::ios::basefield );
    ofs.setf (std::ios::uppercase);
    types::word paragraphs = len / 16;
    types::word rem = len % 16;
    types::word curr_offs = offset;
    for (types::word p =0; p<paragraphs; p++)
    {
        ofs << std::setw (4) << std::setfill ('0') << seg
            << std::setw (1) << ":"
            << std::setw (4) << std::setfill ('0') << curr_offs
            << std::setw (3) << "   ";
        types::word w = curr_offs + 16;
        for (; curr_offs < w; curr_offs++)
        {
            types::byte v = get_byte (curr_offs);
            ofs << std::setw (2) << std::setfill ('0')
                << (types::word)v
                << std::setw (1) << " ";
        }
        ofs << std::endl;
    }
    if (rem > 0)
    {
        ofs << std::setw (4) << std::setfill ('0') << seg
            << std::setw (1) << ":"
            << std::setw (4) << std::setfill ('0') << curr_offs
            << std::setw (3) << "   ";
        types::word w = curr_offs + rem;
        for (; curr_offs < w; curr_offs++)
        {
            types::byte v = get_byte (curr_offs);
            ofs << std::setw (2) << std::setfill ('0')
                << (types::word)v
                << std::setw (1) << " ";
        }
        ofs << std::endl;
    }
}
// -----------------------------------------------------------------
types::word memarea_c::low_mark  () const
{
    return m_low;
}
// -----------------------------------------------------------------
types::word memarea_c::high_mark () const
{
    return m_high;
}
// -----------------------------------------------------------------
types::word memarea_c::area_size () const
{
    return (m_high - m_low) + 1;
}
// -----------------------------------------------------------------
void memarea_c::set_byte  (types::word offset, types::byte b)
{
    _update_access (offset);
    m_area [offset] = b;
}
// -----------------------------------------------------------------
void memarea_c::set_word  (types::word offset, types::word b)
{
    types::reg16_t r;
    r.data = b;
    set_byte (offset    , R_LOW  (r));
    set_byte (offset + 1, R_HIGH (r));
}
// -----------------------------------------------------------------
void memarea_c::set_dword (types::word offset, types::dword b)
{
    types::reg32_t r;
    r.data = b;
    set_word (offset    , R_LOW  (r).data);
    set_word (offset + 2, R_HIGH (r).data);
}
// -----------------------------------------------------------------
void memarea_c::fill_byte  (types::word offset, types::word count, types::byte b)
{
    for (types::word i = 0; i<count; i++)
    {
        set_byte (offset + i, b);
    }
}
// -----------------------------------------------------------------
void memarea_c::fill_word  (types::word offset, types::word count, types::word b)
{
    for (types::word i = 0; i<count; i++)
    {
        set_word (offset + i, b);
    }
}
// -----------------------------------------------------------------
void memarea_c::fill_dword (types::word offset, types::word count, types::dword b)
{
    for (types::word i = 0; i<count; i++)
    {
        set_dword (offset + i, b);
    }
}
// -----------------------------------------------------------------
types::byte  memarea_c::get_byte  (types::word offset) const
{
    _update_access (offset);
    return m_area [offset];
}
// -----------------------------------------------------------------
types::word  memarea_c::get_word  (types::word offset) const
{
    types::reg16_t r;
    R_LOW (r)  = get_byte (offset);
    R_HIGH (r) = get_byte (offset+1);
    return r.data;
}
// -----------------------------------------------------------------
types::dword memarea_c::get_dword (types::word offset) const
{
    types::reg32_t r;
    R_LOW  (r) = get_word (offset);
    R_HIGH (r) = get_word (offset+2);
    return r.data;
}
// -----------------------------------------------------------------
void memarea_c::_update_access (types::word offset) const
{
    m_low  = std::min (m_low , offset);
    m_high = std::max (m_high, offset);
}
