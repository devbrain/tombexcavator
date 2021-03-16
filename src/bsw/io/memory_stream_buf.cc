//
// Created by igor on 17/02/2021.
//

#include "tomb-excavator/bsw/io/memory_stream_buf.hh"
#include "ios_init.hh"

namespace bsw::io
{

    // ==========================================================================================================
    memory_ios::memory_ios(char* pBuffer, std::streamsize bufferSize)
            :
            m_buf(pBuffer, bufferSize)
    {
        te_ios_init(&m_buf);
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_ios::memory_ios(std::shared_ptr<memory_stream_buf_t::mem_block_t> mem)
    : m_buf(mem)
    {
        te_ios_init(&m_buf);
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_ios::memory_ios(std::unique_ptr<memory_stream_buf_t::mem_block_t>&& mem)
    : m_buf(std::move(mem))
    {
        te_ios_init(&m_buf);
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_ios::~memory_ios()
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_input_stream::memory_input_stream(const char* pBuffer, std::streamsize bufferSize)
            :
            memory_ios(const_cast<char*>(pBuffer), bufferSize),
            std::istream(&m_buf)
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_input_stream::memory_input_stream(std::shared_ptr<memory_stream_buf_t::mem_block_t> mem)
    : memory_ios(mem),
    std::istream (&m_buf)
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    memory_input_stream::memory_input_stream(std::unique_ptr<memory_stream_buf_t::mem_block_t>&& mem)
            : memory_ios(std::move(mem)),
              std::istream (&m_buf)
    {

    }
    // -----------------------------------------------------------------------------------------------------------
    memory_input_stream::~memory_input_stream()
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_output_stream::memory_output_stream(char* pBuffer, std::streamsize bufferSize)
            :
            memory_ios(pBuffer, bufferSize),
            std::ostream(&m_buf)
    {
    }
    // -----------------------------------------------------------------------------------------------------------
    memory_output_stream::~memory_output_stream()
    {
    }

}
