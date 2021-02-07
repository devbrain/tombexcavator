//
// Created by igor on 26/05/2020.
//

#include "in_mem_file_ops.hh"

namespace provider
{
    in_mem_file_ops::in_mem_file_ops(std::vector<std::byte> memory)
    : m_memory(std::move(memory)),
    m_input((const unsigned char*)m_memory.data(), m_memory.size())
    {

    }
    // ------------------------------------------------------------------------------
    in_mem_file_ops::in_mem_file_ops(in_mem_file_ops&& other)
    : m_memory(std::move(other.m_memory)),
    m_input(std::move(other.m_input))
    {

    }
    // ------------------------------------------------------------------------------
    ssize_t in_mem_file_ops::do_read (void* buff, size_t len)
    {
#include <bsw/warn/push.hh>
#include <bsw/warn/sign_conversion>
        auto to_read = (len < m_input.bytes_remains()) ? len : m_input.bytes_remains();
#include <bsw/warn/pop.hh>
        m_input.read_buff((char*)buff, len);
        return to_read;
    }
    // --------------------------------------------------------------------------------
    bool in_mem_file_ops::seek (uint64_t pos, enum whence_type whence)
    {
        try
        {
            switch (whence)
            {
                case whence_type::eVFS_SEEK_SET:
                    m_input.seek(pos);
                    break;
                case whence_type::eVFS_SEEK_END:
                    if (pos > m_memory.size())
                    {
                        return false;
                    }
                    m_input.seek(m_memory.size() - pos);
                    break;
                case whence_type::eVFS_SEEK_CUR:
                    m_input.seek(m_input.tell() + pos);
                    break;
            }
        }
        catch (...)
        {
            return false;
        }
        return true;
    }
    // --------------------------------------------------------------------------------
    uint64_t in_mem_file_ops::tell() const
    {
        return m_input.tell();
    }
} // ns provider