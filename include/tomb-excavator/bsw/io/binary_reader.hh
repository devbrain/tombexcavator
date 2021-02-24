//
// Created by igor on 18/02/2021.
//

#ifndef TOMBEXCAVATOR_BINARY_READER_HH
#define TOMBEXCAVATOR_BINARY_READER_HH

#include <cstdint>
#include <vector>
#include <istream>

#include <tomb-excavator/tombexcavator_configure.h>
#include "memory_stream_buf.hh"
#include "tomb-excavator/bsw_export.h"

namespace bsw::io
{
    class BSW_API binary_reader
        /// This class reads basic types (and std::vectors thereof)
        /// in binary form into an input stream.
        /// It provides an extractor-based interface similar to istream.
        /// The reader also supports automatic conversion from big-endian
        /// (network byte order) to little-endian and vice-versa.
        /// Use a BinaryWriter to create a stream suitable for a BinaryReader.
    {
    public:
        enum stream_byte_order
        {
            NATIVE_BYTE_ORDER = 1,  /// the host's native byte-order
            BIG_ENDIAN_BYTE_ORDER = 2,  /// big-endian (network) byte-order
            LITTLE_ENDIAN_BYTE_ORDER = 3,  /// little-endian byte-order
        };

        explicit binary_reader(std::istream& istr, stream_byte_order byteOrder = NATIVE_BYTE_ORDER);
        /// Creates the BinaryReader.



        ~binary_reader();
        /// Destroys the BinaryReader.

        binary_reader& operator>>(bool& value);
        binary_reader& operator>>(char& value);
        binary_reader& operator>>(unsigned char& value);
        binary_reader& operator>>(signed char& value);
        binary_reader& operator>>(short& value);
        binary_reader& operator>>(unsigned short& value);
        binary_reader& operator>>(int& value);
        binary_reader& operator>>(unsigned int& value);
        binary_reader& operator>>(long& value);
        binary_reader& operator>>(unsigned long& value);
        binary_reader& operator>>(float& value);
        binary_reader& operator>>(double& value);

        binary_reader& operator>>(long long& value);
        binary_reader& operator>>(unsigned long long& value);

        binary_reader& operator>>(std::vector<char>& value);
        binary_reader& operator>>(std::vector<unsigned char>& value);

        void read_raw(std::streamsize length, std::string& value);
        /// Reads length bytes of raw data into value.

        void read_raw(char* buffer, std::streamsize length);
        /// Reads length bytes of raw data into buffer.

        void read_raw(unsigned char* buffer, std::streamsize length);
        /// Reads length bytes of raw data into buffer.

        bool good();
        /// Returns _istr.good();

        bool fail();
        /// Returns _istr.fail();

        bool bad();
        /// Returns _istr.bad();

        bool eof();
        /// Returns _istr.eof();

        [[nodiscard]] std::istream& stream() const;
        /// Returns the underlying stream.

        [[nodiscard]] stream_byte_order byte_order() const;
        /// Returns the byte-order used by the reader, which is
        /// either BIG_ENDIAN_BYTE_ORDER or LITTLE_ENDIAN_BYTE_ORDER.

        void set_exceptions(std::ios_base::iostate st = (std::istream::failbit | std::istream::badbit));
        /// Sets the stream to throw exception on specified state (default failbit and badbit);

        [[nodiscard]] std::streamsize available() const;
        /// Returns the number of available bytes in the stream.

    private:
        std::istream& m_istr;
        bool m_flip_bytes;
    };

    template<typename T>
    class basic_memory_binary_reader : public binary_reader
        /// A convenient wrapper for using Buffer and MemoryStream with BinaryReader.
    {
    public:
        basic_memory_binary_reader(const char* data, std::size_t size, stream_byte_order byteOrder = NATIVE_BYTE_ORDER)
                :
                binary_reader(m_istr, byteOrder),
                m_istr(data, size)
        {
        }

        const memory_input_stream& stream() const
        {
            return m_istr;
        }

        memory_input_stream& stream()
        {
            return m_istr;
        }

    private:
        memory_input_stream m_istr;
    };

    typedef basic_memory_binary_reader<char> memory_binary_reader;


//
// inlines
//


    inline bool binary_reader::good()
    {
        return m_istr.good();
    }

    inline bool binary_reader::fail()
    {
        return m_istr.fail();
    }

    inline bool binary_reader::bad()
    {
        return m_istr.bad();
    }

    inline bool binary_reader::eof()
    {
        return m_istr.eof();
    }

    inline std::istream& binary_reader::stream() const
    {
        return m_istr;
    }

    inline binary_reader::stream_byte_order binary_reader::byte_order() const
    {
#if !TE_IS_LITTLE_ENDIAN
        return _flip_bytes ? LITTLE_ENDIAN_BYTE_ORDER : BIG_ENDIAN_BYTE_ORDER;
#else
        return m_flip_bytes ? BIG_ENDIAN_BYTE_ORDER : LITTLE_ENDIAN_BYTE_ORDER;
#endif
    }

    inline void binary_reader::set_exceptions(std::ios_base::iostate st)
    {
        m_istr.exceptions(st);
    }

    inline std::streamsize binary_reader::available() const
    {
        return m_istr.rdbuf()->in_avail();
    }

}

#endif //TOMBEXCAVATOR_BINARY_READER_HH
