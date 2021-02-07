//
// Created by igor on 07/02/2021.
//

#include <sstream>
#include <stdexcept>
#include <cstring>
#include "output.hh"


namespace formats
{
    file_output::file_output(const std::filesystem::path& path)
            : m_owner(true)
    {
        m_file = fopen(path.c_str(), "wb");
        if (!m_file)
        {
            std::ostringstream  os;
            os << "Failed to open file " << path.string();
            throw std::runtime_error(os.str());
        }
    }
    // -------------------------------------------------------------
    file_output::file_output(FILE* file)
            : m_owner(false),
              m_file(file)
    {
    }
    // -------------------------------------------------------------
    file_output::~file_output()
    {
        if (m_owner)
        {
            fclose(m_file);
        }
    }
    // -------------------------------------------------------------
    void file_output::write_buff(const char* buffer, std::size_t size)
    {
        if (fwrite(buffer, size, 1, m_file) != 1)
        {
            throw std::runtime_error ("write failed");
        }
    }
    // -------------------------------------------------------------
    std::size_t file_output::tell() const
    {
        const long pos = ftell(m_file);
        if (pos <= 0)
        {
            throw std::runtime_error("tell failed");
        }
        return static_cast <std::size_t> (pos);
    }
    // -------------------------------------------------------------
    void file_output::seek(std::size_t offset)
    {
        if (fseek(m_file, offset, SEEK_SET) != 0)
        {
            throw std::runtime_error("seek failed");
        }
    }
    // =============================================================
    memory_output::memory_output(std::vector<char>& out_buff)
            : m_buff(out_buff),
              m_ptr(out_buff.size())
    {

    }
    // -------------------------------------------------------------
    void memory_output::write_buff(const char* buffer, std::size_t size)
    {
        if (m_ptr + size >= m_buff.size())
        {
            m_buff.resize(m_ptr + size);
        }
        std::memcpy(&m_buff[m_ptr], buffer, size);
        m_ptr += size;
    }
    // -------------------------------------------------------------
    std::size_t memory_output::tell() const
    {
        return static_cast <std::size_t> (m_ptr);
    }
    // -------------------------------------------------------------
    void memory_output::seek(std::size_t offset)
    {
        if (offset >= m_buff.size())
        {
            std::ostringstream os;
            os << "Attempt to seek after the end of the write buffer. Size = "
            << m_buff.size() << " offset = " << offset;
            throw std::runtime_error(os.str());
        }
        m_ptr = offset;
    }
}