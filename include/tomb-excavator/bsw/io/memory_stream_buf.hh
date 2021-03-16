//
// Created by igor on 17/02/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_MEMORY_STREAM_BUF_HH
#define TOMBEXCAVATOR_ARCHIVE_MEMORY_STREAM_BUF_HH

#include <streambuf>
#include <ios>
#include <istream>
#include <ostream>
#include <memory>
#include <vector>
#include <stdexcept>

#include <tomb-excavator/export-bsw.h>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace bsw::io
{
    template<typename ch, typename tr>
    class memory_stream_buf : public std::basic_streambuf<ch, tr>
        /// BasicMemoryStreamBuf is a simple implementation of a
        /// stream buffer for reading and writing from a memory area.
        ///
        /// This streambuf only supports unidirectional streams.
        /// In other words, the BasicMemoryStreamBuf can be
        /// used for the implementation of an istream or an
        /// ostream, but not for an iostream.
    {
    protected:
        typedef std::basic_streambuf<ch, tr> Base;
        typedef std::basic_ios<ch, tr> IOS;
        typedef ch char_type;
        typedef tr char_traits;
        typedef typename Base::int_type int_type;
        typedef typename Base::pos_type pos_type;
        typedef typename Base::off_type off_type;
    public:
        typedef std::vector<char_type> mem_block_t;
    public:
        memory_stream_buf() = delete;
        memory_stream_buf(const memory_stream_buf&) = delete;
        memory_stream_buf& operator=(const memory_stream_buf&) = delete;

        memory_stream_buf(char_type* pBuffer, std::streamsize bufferSize);
        explicit memory_stream_buf(std::shared_ptr<mem_block_t> mem);
        explicit memory_stream_buf(std::unique_ptr<mem_block_t>&& mem);
        ~memory_stream_buf();


        virtual int_type overflow(int_type /*c*/);
        virtual int_type underflow();
        virtual pos_type seekoff(off_type off, std::ios_base::seekdir way,
                                 std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
        virtual int sync();
        std::streamsize chars_written() const;

        /// Resets the buffer so that current read and write positions
        /// will be set to the beginning of the buffer.
        void reset();
    private:
        char_type* m_buffer;
        std::streamsize m_buffer_size;
        std::shared_ptr<mem_block_t> m_mem;
    };

//
// We provide an instantiation for char
//
    typedef memory_stream_buf<char, std::char_traits<char>> memory_stream_buf_t;

    class BSW_API memory_ios : public virtual std::ios
        /// The base class for MemoryInputStream and MemoryOutputStream.
        ///
        /// This class is needed to ensure the correct initialization
        /// order of the stream buffer and base classes.
    {
    public:
        memory_ios(char* pBuffer, std::streamsize bufferSize);
        explicit memory_ios(std::shared_ptr<memory_stream_buf_t::mem_block_t> mem);
        explicit memory_ios(std::unique_ptr<memory_stream_buf_t::mem_block_t>&& mem);
        /// Creates the basic stream.

        ~memory_ios();
        /// Destroys the stream.

        memory_stream_buf_t* rdbuf();
        /// Returns a pointer to the underlying streambuf.

    protected:
        memory_stream_buf_t m_buf;
    };

    class BSW_API memory_input_stream : public memory_ios, public std::istream
/// An input stream for reading from a memory area.
    {
    public:
        memory_input_stream(const char* pBuffer, std::streamsize bufferSize);
        explicit memory_input_stream(std::shared_ptr<memory_stream_buf_t::mem_block_t> mem);
        explicit memory_input_stream(std::unique_ptr<memory_stream_buf_t::mem_block_t>&& mem);
/// Creates a MemoryInputStream for the given memory area,
/// ready for reading.

        ~memory_input_stream();
/// Destroys the MemoryInputStream.
    };

    class BSW_API memory_output_stream : public memory_ios, public std::ostream
/// An input stream for reading from a memory area.
    {
    public:
        memory_output_stream(char* pBuffer, std::streamsize bufferSize);
/// Creates a MemoryOutputStream for the given memory area,
/// ready for writing.

        ~memory_output_stream();
/// Destroys the MemoryInputStream.

        std::streamsize charsWritten() const;
/// Returns the number of chars written to the buffer.
    };



//
// inlines
//
    template<typename ch, typename tr>
    memory_stream_buf<ch, tr>::memory_stream_buf(char_type* pBuffer, std::streamsize bufferSize)
            :
            m_buffer(pBuffer),
            m_buffer_size(bufferSize)
    {
        this->setg(m_buffer, m_buffer, m_buffer + m_buffer_size);
        this->setp(m_buffer, m_buffer + m_buffer_size);
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    memory_stream_buf<ch, tr>::memory_stream_buf(std::shared_ptr<mem_block_t> mem)
    : m_mem(mem)
    {
        m_buffer = m_mem->data();
        m_buffer_size = m_mem->size();
        this->setg(m_buffer, m_buffer, m_buffer + m_buffer_size);
        this->setp(m_buffer, m_buffer + m_buffer_size);
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    memory_stream_buf<ch, tr>::memory_stream_buf(std::unique_ptr<mem_block_t>&& mem)
            : m_mem(std::move(mem))
    {
        m_buffer = m_mem->data();
        m_buffer_size = m_mem->size();
        this->setg(m_buffer, m_buffer, m_buffer + m_buffer_size);
        this->setp(m_buffer, m_buffer + m_buffer_size);
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    memory_stream_buf<ch, tr>::~memory_stream_buf()
    {
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    typename memory_stream_buf<ch, tr>::int_type memory_stream_buf<ch, tr>::overflow(int_type /*c*/)
    {
        return char_traits::eof();
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    typename memory_stream_buf<ch, tr>::int_type memory_stream_buf<ch, tr>::underflow()
    {
        return char_traits::eof();
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    typename memory_stream_buf<ch, tr>::pos_type memory_stream_buf<ch, tr>::seekoff(off_type off, std::ios_base::seekdir way,
                                                                                    std::ios_base::openmode which)
    {
        const pos_type fail = off_type(-1);
        off_type newoff = off_type(-1);

        if ((which & std::ios_base::in) != 0)
        {
            if (this->gptr() == 0)
            {
                return fail;
            }

            if (way == std::ios_base::beg)
            {
                newoff = 0;
            } else
            {
                if (way == std::ios_base::cur)
                {
                    // cur is not valid if both in and out are specified (Condition 3)
                    if ((which & std::ios_base::out) != 0)
                    {
                        return fail;
                    }
                    newoff = this->gptr() - this->eback();
                } else
                {
                    if (way == std::ios_base::end)
                    {
                        newoff = this->egptr() - this->eback();
                    } else
                    {
                        throw std::runtime_error("should not be here");
                    }
                }
            }

            if ((newoff + off) < 0 || (this->egptr() - this->eback()) < (newoff + off))
            {
                return fail;
            }
            this->setg(this->eback(), this->eback() + newoff + off, this->egptr());
        }

        if ((which & std::ios_base::out) != 0)
        {
            if (this->pptr() == 0)
            {
                return fail;
            }

            if (way == std::ios_base::beg)
            {
                newoff = 0;
            } else
            {
                if (way == std::ios_base::cur)
                {
                    // cur is not valid if both in and out are specified (Condition 3)
                    if ((which & std::ios_base::in) != 0)
                    {
                        return fail;
                    }
                    newoff = this->pptr() - this->pbase();
                } else
                {
                    if (way == std::ios_base::end)
                    {
                        newoff = this->epptr() - this->pbase();
                    } else
                    {
                        throw std::runtime_error("should not be here");
                    }
                }
            }

            if (newoff + off < 0 || (this->epptr() - this->pbase()) < newoff + off)
            {
                return fail;
            }
            this->pbump((int) (newoff + off - (this->pptr() - this->pbase())));
        }

        return newoff;
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    int memory_stream_buf<ch, tr>::sync()
    {
        return 0;
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    std::streamsize memory_stream_buf<ch, tr>::chars_written() const
    {
        return static_cast<std::streamsize>(this->pptr() - this->pbase());
    }
    // -----------------------------------------------------------------------------------------
    template<typename ch, typename tr>
    void memory_stream_buf<ch, tr>::reset()
    {
        this->setg(m_buffer, m_buffer, m_buffer + m_buffer_size);
        this->setp(m_buffer, m_buffer + m_buffer_size);
    }
    // ======================================================================================
    inline memory_stream_buf_t* memory_ios::rdbuf()
    {
        return &m_buf;
    }
    // ======================================================================================
    inline std::streamsize memory_output_stream::charsWritten() const
    {
        return m_buf.chars_written();
    }
}

#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_MEMORY_STREAM_BUF_HH
