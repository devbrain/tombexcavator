//
// Created by igor on 17/02/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_BUFFERED_STREAM_BUF_HH
#define TOMBEXCAVATOR_ARCHIVE_BUFFERED_STREAM_BUF_HH

#include <streambuf>
#include <iosfwd>
#include <ios>
#include <stdexcept>

namespace bsw::io
{
    template <typename ch, typename tr>
    class basic_buffered_stream_buf: public std::basic_streambuf<ch, tr>
        /// This is an implementation of a buffered streambuf
        /// that greatly simplifies the implementation of
        /// custom streambufs of various kinds.
        /// Derived classes only have to override the methods
        /// read_from_device() or write_to_device().
        ///
        /// This streambuf only supports unidirectional streams.
        /// In other words, the BasicBufferedStreamBuf can be
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
        typedef typename IOS::openmode openmode;

    public:
        basic_buffered_stream_buf(std::streamsize bufferSize, openmode mode):
                _bufsize(bufferSize),
                _pBuffer(new char_type [_bufsize]),
                _mode(mode)
        {
            this->setg(_pBuffer + 4, _pBuffer + 4, _pBuffer + 4);
            this->setp(_pBuffer, _pBuffer + _bufsize);
        }

        ~basic_buffered_stream_buf() override
        {
            delete [] _pBuffer;
        }

        basic_buffered_stream_buf(const basic_buffered_stream_buf&) = delete;
        basic_buffered_stream_buf& operator = (const basic_buffered_stream_buf&) = delete;

        virtual int_type overflow(int_type c)
        {
            if (!(_mode & IOS::out)) return char_traits::eof();

            if (flushBuffer() == std::streamsize(-1)) return char_traits::eof();
            if (c != char_traits::eof())
            {
                *this->pptr() = char_traits::to_char_type(c);
                this->pbump(1);
            }

            return c;
        }

        virtual int_type underflow()
        {
            if (!(_mode & IOS::in)) return char_traits::eof();

            if (this->gptr() && (this->gptr() < this->egptr()))
                return char_traits::to_int_type(*this->gptr());

            int putback = int(this->gptr() - this->eback());
            if (putback > 4) putback = 4;

            char_traits::move(_pBuffer + (4 - putback), this->gptr() - putback, putback);

            int n = read_from_device(_pBuffer + 4, _bufsize - 4);
            if (n <= 0) return char_traits::eof();

            this->setg(_pBuffer + (4 - putback), _pBuffer + 4, _pBuffer + 4 + n);

            // return next character
            return char_traits::to_int_type(*this->gptr());
        }

        virtual int sync()
        {
            if (this->pptr() && this->pptr() > this->pbase())
            {
                if (flushBuffer() == -1) return -1;
            }
            return 0;
        }

    protected:
        void setMode(openmode mode)
        {
            _mode = mode;
        }

        openmode getMode() const
        {
            return _mode;
        }

    private:
        virtual int read_from_device(char_type* /*buffer*/, std::streamsize /*length*/)
        {
            return 0;
        }

        virtual int write_to_device(const char_type* /*buffer*/, std::streamsize /*length*/)
        {
            return 0;
        }

        int flushBuffer()
        {
            int n = int(this->pptr() - this->pbase());
            if (write_to_device(this->pbase(), n) == n)
            {
                this->pbump(-n);
                return n;
            }
            return -1;
        }

        std::streamsize _bufsize;
        char_type*      _pBuffer;
        openmode        _mode;
    };
}

#endif
