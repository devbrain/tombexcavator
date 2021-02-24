//
// Created by igor on 18/02/2021.
//

#ifndef TOMBEXCAVATOR_BINARY_WRITER_HH
#define TOMBEXCAVATOR_BINARY_WRITER_HH


#include <vector>
#include <ostream>
#include <tomb-excavator/bsw_export.h>
#include <tomb-excavator/tombexcavator_configure.h>
#include <tomb-excavator/bsw/io/memory_stream_buf.hh>

namespace bsw::io
{


    class BSW_API binary_writer
        /// This class writes basic types (and std::vectors of these)
        /// in binary form into an output stream.
        /// It provides an inserter-based interface similar to ostream.
        /// The writer also supports automatic conversion from big-endian
        /// (network byte order) to little-endian and vice-versa.
        /// Use a BinaryReader to read from a stream created by a BinaryWriter.
        /// Be careful when exchanging data between systems with different
        /// data type sizes (e.g., 32-bit and 64-bit architectures), as the sizes
        /// of some of the basic types may be different. For example, writing a
        /// long integer on a 64-bit system and reading it on a 32-bit system
        /// may yield an incorrent result. Use fixed-size types (Int32, Int64, etc.)
        /// in such a case.
    {
    public:
        enum stream_byte_order_t
        {
            NATIVE_BYTE_ORDER = 1, /// the host's native byte-order
            BIG_ENDIAN_BYTE_ORDER = 2, /// big-endian (network) byte-order
            LITTLE_ENDIAN_BYTE_ORDER = 3  /// little-endian byte-order
        };

        explicit binary_writer(std::ostream& ostr, stream_byte_order_t byteOrder = NATIVE_BYTE_ORDER);
        /// Creates the BinaryWriter.



        ~binary_writer();
        /// Destroys the BinaryWriter.

        binary_writer& operator<<(bool value);
        binary_writer& operator<<(char value);
        binary_writer& operator<<(unsigned char value);
        binary_writer& operator<<(signed char value);
        binary_writer& operator<<(short value);
        binary_writer& operator<<(unsigned short value);
        binary_writer& operator<<(int value);
        binary_writer& operator<<(unsigned int value);
        binary_writer& operator<<(long value);
        binary_writer& operator<<(unsigned long value);
        binary_writer& operator<<(float value);
        binary_writer& operator<<(double value);

        binary_writer& operator<<(long long value);
        binary_writer& operator<<(unsigned long long value);

        void write_raw(const std::string& rawData);
        /// Writes the string as-is to the stream.

        void write_raw(const char* buffer, std::streamsize length);
        /// Writes length raw bytes from the given buffer to the stream.


        void flush();
        /// Flushes the underlying stream.

        bool good();
        /// Returns _ostr.good();

        bool fail();
        /// Returns _ostr.fail();

        bool bad();
        /// Returns _ostr.bad();

        [[nodiscard]] std::ostream& stream() const;
        /// Returns the underlying stream.

        [[nodiscard]] stream_byte_order_t byteOrder() const;
        /// Returns the byte ordering used by the writer, which is
        /// either BIG_ENDIAN_BYTE_ORDER or LITTLE_ENDIAN_BYTE_ORDER.

    private:
        std::ostream& _ostr;
        bool _flipBytes;
    };

    template<typename T>
    class basic_memory_binary_writer : public binary_writer
        /// A convenient wrapper for using Buffer and MemoryStream with BinarWriter.
    {
    public:
        basic_memory_binary_writer(char* data, std::size_t size, stream_byte_order_t byteOrder = NATIVE_BYTE_ORDER)
                :
                binary_writer(_ostr, byteOrder),
                _ostr(data, size)
        {
        }

        ~basic_memory_binary_writer()
        {
            try
            {
                flush();
            }
            catch (...)
            {

            }
        }

        const memory_output_stream& stream() const
        {
            return _ostr;
        }

        memory_output_stream& stream()
        {
            return _ostr;
        }

    private:
        memory_output_stream _ostr;
    };

    typedef basic_memory_binary_writer<char> memory_binary_writer;


//
// inlines
//


    inline std::ostream& binary_writer::stream() const
    {
        return _ostr;
    }

    inline bool binary_writer::good()
    {
        return _ostr.good();
    }

    inline bool binary_writer::fail()
    {
        return _ostr.fail();
    }

    inline bool binary_writer::bad()
    {
        return _ostr.bad();
    }

    inline binary_writer::stream_byte_order_t binary_writer::byteOrder() const
    {
#if !TE_IS_LITTLE_ENDIAN
        return _flipBytes ? LITTLE_ENDIAN_BYTE_ORDER : BIG_ENDIAN_BYTE_ORDER;
#else
        return _flipBytes ? BIG_ENDIAN_BYTE_ORDER : LITTLE_ENDIAN_BYTE_ORDER;
#endif
    }


} // ns



#endif //TOMBEXCAVATOR_BINARY_WRITER_HH
