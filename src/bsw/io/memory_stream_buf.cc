//
// Created by igor on 17/02/2021.
//

#include "tomb-excavator/bsw/io/memory_stream_buf.hh"
#include "ios_init.hh"

namespace bsw::io
{
    memory_ios::memory_ios(char* pBuffer, std::streamsize bufferSize)
            :
            _buf(pBuffer, bufferSize)
    {
        te_ios_init(&_buf);
    }

    memory_ios::~memory_ios()
    {
    }

    memory_input_stream::memory_input_stream(const char* pBuffer, std::streamsize bufferSize)
            :
            memory_ios(const_cast<char*>(pBuffer), bufferSize),
            std::istream(&_buf)
    {
    }

    memory_input_stream::~memory_input_stream()
    {
    }

    memory_output_stream::memory_output_stream(char* pBuffer, std::streamsize bufferSize)
            :
            memory_ios(pBuffer, bufferSize),
            std::ostream(&_buf)
    {
    }

    memory_output_stream::~memory_output_stream()
    {
    }

}
