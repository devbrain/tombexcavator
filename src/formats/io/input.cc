//
// Created by igor on 06/02/2021.
//

#include <cstring>
#include <stdexcept>
#include <sstream>
#include "input.hh"

namespace formats
{
    void base_memory_input::read_buff (char* buffer, std::size_t size)
    {
        if (m_offset + size < m_size)
        {
            std::memcpy(buffer, m_buffer + m_offset, size);
            m_offset += size;
        }
        else
        {
            std::ostringstream os;
            os << "Attempt to read after the end of the input. Size = "
            << size << " offset = " << m_offset << " to read = " << size;
            throw std::runtime_error(os.str());
        }
    }
    // -------------------------------------------------------------------------------------
    std::size_t base_memory_input::tell () const
    {
        return m_offset;
    }
    // -------------------------------------------------------------------------------------
    std::size_t base_memory_input::bytes_remains () const
    {
        return m_size - m_offset;
    }
    // -------------------------------------------------------------------------------------
    void base_memory_input::seek (std::size_t offset)
    {
        if (offset > m_size)
        {
            std::ostringstream os;
            os << "Attempt to set current offset after end of the input. "
            << "Size = " << m_size << " offset = " << offset;
            throw std::runtime_error(os.str());
        }
        m_offset = offset;
    }
    // -------------------------------------------------------------------------------------
    void base_memory_input::setup(const char* buff, std::size_t size)
    {
        m_buffer = buff;
        m_size = size;
        m_offset = 0;
    }
    // -------------------------------------------------------------------------------------
    base_memory_input::base_memory_input()
    {
        setup(nullptr, 0);
    }
    // =====================================================================================
    file_input::file_input(const std::filesystem::path& path)
    : m_source(path.string())
    {
        setup(m_source.data(), m_source.size());
    }
    // =====================================================================================
    memory_input::memory_input(const char* buff, std::size_t size)
    {
        setup(buff, size);
    }
    // -------------------------------------------------------------------------------------
    memory_input::memory_input(const std::vector<char>& buff)
    {
        setup(buff.data(), buff.size());
    }
} // ns formats