//
// Created by igor on 17/02/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_UNBUFFERED_STREAM_BUF_HH
#define TOMBEXCAVATOR_ARCHIVE_UNBUFFERED_STREAM_BUF_HH

#include <streambuf>
#include <iosfwd>
#include <ios>

namespace bsw::io
{
    template<typename ch, typename tr>
    class basic_unbuffered_stream_buf : public std::basic_streambuf<ch, tr>
        /// This is an implementation of an unbuffered streambuf
        /// that greatly simplifies the implementation of
        /// custom streambufs of various kinds.
        /// Derived classes only have to override the methods
        /// read_from_device() or write_to_device().
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
        basic_unbuffered_stream_buf()
                :
                _pb(char_traits::eof()),
                _ispb(false)
        {
            this->setg(0, 0, 0);
            this->setp(0, 0);
        }

        ~basic_unbuffered_stream_buf()
        {
        }

        basic_unbuffered_stream_buf(const basic_unbuffered_stream_buf&) = delete;
        basic_unbuffered_stream_buf& operator=(const basic_unbuffered_stream_buf&) = delete;

        virtual int_type overflow(int_type c)
        {
            if (c != char_traits::eof())
            {
                return write_to_device(char_traits::to_char_type(c));
            } else
            {
                return c;
            }
        }

        virtual int_type underflow()
        {
            if (_ispb)
            {
                return _pb;
            } else
            {
                int_type c = read_from_device();
                if (c != char_traits::eof())
                {
                    _ispb = true;
                    _pb = c;
                }
                return c;
            }
        }

        virtual int_type uflow()
        {
            if (_ispb)
            {
                _ispb = false;
                return _pb;
            } else
            {
                int_type c = read_from_device();
                if (c != char_traits::eof())
                {
                    _pb = c;
                }
                return c;
            }
        }

        virtual int_type pbackfail(int_type c)
        {
            if (_ispb)
            {
                return char_traits::eof();
            } else
            {
                _ispb = true;
                _pb = c;
                return c;
            }
        }

        virtual std::streamsize xsgetn(char_type* p, std::streamsize count)
        /// Some platforms (for example, Compaq C++) have buggy implementations of
        /// xsgetn that handle null buffers incorrectly.
        /// Anyway, it does not hurt to provide an optimized implementation
        /// of xsgetn for this streambuf implementation.
        {
            std::streamsize copied = 0;
            while (count > 0)
            {
                int_type c = uflow();
                if (c == char_traits::eof())
                { break; }
                *p++ = char_traits::to_char_type(c);
                ++copied;
                --count;
            }
            return copied;
        }

    protected:
        static int_type charToInt(char_type c)
        {
            return char_traits::to_int_type(c);
        }

    private:
        virtual int_type read_from_device()
        {
            return char_traits::eof();
        }

        virtual int_type write_to_device(char_type)
        {
            return char_traits::eof();
        }

        int_type _pb;
        bool _ispb;
    };
} // ns

#endif
