//
// Created by igor on 11/04/2021.
//

#include "chunked_file.hh"
#include <tomb-excavator/bsw/exceptions.hh>

chunked_file::chunked_file(std::istream& is, std::size_t entry_size)
: m_is(is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER),
  m_entry_size(entry_size)
{
    m_offset = m_is.stream().tellg();
    uint32_t header;
    m_is >> header;
    constexpr static auto FORM = formats::iff::fourcc("FORM");
    if (formats::iff::chunk_type(header) != FORM)
    {
        RAISE_EX("FORM expected");
    }
    uint32_t file_size;
    m_is >> file_size;
    m_file_size = file_size;
    if (m_file_size > m_entry_size)
    {
        RAISE_EX("Chunked file is corrupted. Reported size = ", m_file_size, " entry size = ", m_entry_size);
    }
    m_data_offset = m_offset + 12;
}
// ------------------------------------------------------------------------------------------
chunked_file::~chunked_file()
{
    m_is.stream().seekg(m_offset, std::ios::beg);
}
// ------------------------------------------------------------------------------------------
std::optional<chunked_file::offset_and_size_t> chunked_file::find_section(const formats::iff::chunk_type& type)
{
    m_is.stream().seekg(m_data_offset, std::ios::beg);
    uint64_t pos = 0;
    while (pos + 8 < m_file_size)
    {
        uint32_t header;
        m_is >> header;
        uint32_t size;
        m_is >> size;
        pos += 8;
        if (formats::iff::chunk_type(header) == type)
        {
            return offset_and_size_t (m_data_offset + pos, size);
        }
        pos += size;
        if (pos < m_file_size)
        {
            m_is.stream().seekg(m_data_offset + pos, std::ios::beg);
        }
    }
    return std::nullopt;
}
// ------------------------------------------------------------------------------------------