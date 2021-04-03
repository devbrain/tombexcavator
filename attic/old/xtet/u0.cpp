#include "u0.hpp"


u0_c::u0_c (std::ifstream& ifs, const header_c& hdr)
{
    m_num_items = hdr.u0_chunks ();
    const size_t u0_area_size = m_num_items * 32;
    m_buff = new char [u0_area_size];
    ifs.seekg (hdr.u0_offset (),  std::ios_base::beg);
    ifs.read (m_buff, u0_area_size);
    // transmogrify
    unsigned int* array = (unsigned int*)m_buff;
    unsigned int items_counter = 0;
    unsigned int items_ptr = 0;
    while (items_counter < m_num_items)
    {
        if ( array [5] != 0)
        {
            array [items_ptr + 6] |= 1;
        }
        array [items_ptr + 5] = 0;
        items_ptr += 8;
        array [items_ptr - 1] = 0;

        items_counter ++;
    }
}
// ----------------------------------------------------------------------------
u0_c::~u0_c ()
{
    delete [] m_buff;
}
