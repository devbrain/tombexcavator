//
// Created by igor on 18/02/2021.
//

#include "tomb-excavator/bsw/io/binary_reader.hh"
#include "tomb-excavator/bsw/byte_order.hh"

namespace bsw::io
{
    binary_reader::binary_reader(std::istream& istr, stream_byte_order byteOrder)
            : m_istr(istr)
    {
#if !TE_IS_LITTLE_ENDIAN
        m_flip_bytes = (byteOrder == LITTLE_ENDIAN_BYTE_ORDER);
#else
        m_flip_bytes = (byteOrder == BIG_ENDIAN_BYTE_ORDER);
#endif
    }

    binary_reader::~binary_reader()
    {
    }

    binary_reader& binary_reader::operator>>(bool& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        return *this;
    }

    binary_reader& binary_reader::operator>>(char& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        return *this;
    }

    binary_reader& binary_reader::operator>>(unsigned char& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        return *this;
    }

    binary_reader& binary_reader::operator>>(signed char& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        return *this;
    }

    binary_reader& binary_reader::operator>>(short& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if (m_flip_bytes)
        { value = byte_order::flip_bytes(value); }
        return *this;
    }

    binary_reader& binary_reader::operator>>(unsigned short& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if (m_flip_bytes)
        { value = byte_order::flip_bytes(value); }
        return *this;
    }

    binary_reader& binary_reader::operator>>(int& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if (m_flip_bytes)
        { value = byte_order::flip_bytes(value); }
        return *this;
    }

    binary_reader& binary_reader::operator>>(unsigned int& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if (m_flip_bytes)
        { value = byte_order::flip_bytes(value); }
        return *this;
    }

    binary_reader& binary_reader::operator>>(long& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if constexpr (sizeof(long) == 8)
        {
            if (m_flip_bytes)
            { value = (long)byte_order::flip_bytes((int64_t) value); }
        } else
        {
            if (m_flip_bytes)
            { value = (long)byte_order::flip_bytes((int32_t) value); }
        }

        return *this;
    }

    binary_reader& binary_reader::operator>>(unsigned long& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if constexpr (sizeof(unsigned long) == 8)
        {
            if (m_flip_bytes)
            { value = (unsigned long)byte_order::flip_bytes((uint64_t) value); }
        } else
        {
            if (m_flip_bytes)
            { value = (unsigned long)byte_order::flip_bytes((uint32_t) value); }
        }
        return *this;
    }

    binary_reader& binary_reader::operator>>(float& value)
    {
        if (m_flip_bytes)
        {
            char* ptr = (char*) &value;
            ptr += sizeof(value);
            for (unsigned i = 0; i < sizeof(value); ++i)
            {
                m_istr.read(--ptr, 1);
            }
        } else
        {
            m_istr.read((char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_reader& binary_reader::operator>>(double& value)
    {
        if (m_flip_bytes)
        {
            char* ptr = (char*) &value;
            ptr += sizeof(value);
            for (unsigned i = 0; i < sizeof(value); ++i)
            {
                m_istr.read(--ptr, 1);
            }
        } else
        {
            m_istr.read((char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_reader& binary_reader::operator>>(long long& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if (m_flip_bytes)
        { value = byte_order::flip_bytes(static_cast<int64_t>(value)); }
        return *this;
    }

    binary_reader& binary_reader::operator>>(unsigned long long& value)
    {
        m_istr.read((char*) &value, sizeof(value));
        if (m_flip_bytes)
        { value = byte_order::flip_bytes(static_cast<uint64_t>(value)); }
        return *this;
    }

    binary_reader& binary_reader::operator>>(std::vector<char>& value)
    {
        read_raw(value.data(), value.size());
        return *this;
    }

    binary_reader& binary_reader::operator>>(std::vector<unsigned char>& value)
    {
        read_raw(value.data(), value.size());
        return *this;
    }

    void binary_reader::read_raw(std::streamsize length, std::string& value)
    {
        value.clear();
        value.reserve(static_cast<std::string::size_type>(length));
        while (length--)
        {
            char c;
            if (!m_istr.read(&c, 1).good())
            { break; }
            value += c;
        }
    }

    void binary_reader::read_raw(char* buffer, std::streamsize length)
    {
        m_istr.read(buffer, length);
    }

    void binary_reader::read_raw(unsigned char* buffer, std::streamsize length)
    {
        m_istr.read((char *)buffer, length);
    }

} // ns
