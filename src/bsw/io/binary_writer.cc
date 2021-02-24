//
// Created by igor on 18/02/2021.
//

#include "tomb-excavator/bsw/io/binary_writer.hh"
#include "tomb-excavator/bsw/byte_order.hh"

namespace bsw::io
{
    binary_writer::binary_writer(std::ostream& ostr, stream_byte_order_t byteOrder)
            :
            _ostr(ostr)
    {
#if !TE_IS_LITTLE_ENDIAN
        _flipBytes = (byteOrder == LITTLE_ENDIAN_BYTE_ORDER);
#else
        _flipBytes = (byteOrder == BIG_ENDIAN_BYTE_ORDER);
#endif
    }

    binary_writer::~binary_writer()
    {

    }

    binary_writer& binary_writer::operator<<(bool value)
    {
        _ostr.write((const char*) &value, sizeof(value));
        return *this;
    }

    binary_writer& binary_writer::operator<<(char value)
    {
        _ostr.write((const char*) &value, sizeof(value));
        return *this;
    }

    binary_writer& binary_writer::operator<<(unsigned char value)
    {
        _ostr.write((const char*) &value, sizeof(value));
        return *this;
    }

    binary_writer& binary_writer::operator<<(signed char value)
    {
        _ostr.write((const char*) &value, sizeof(value));
        return *this;
    }

    binary_writer& binary_writer::operator<<(short value)
    {
        if (_flipBytes)
        {
            short fValue = byte_order::flip_bytes(value);
            _ostr.write((const char*) &fValue, sizeof(fValue));
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(unsigned short value)
    {
        if (_flipBytes)
        {
            unsigned short fValue = byte_order::flip_bytes(value);
            _ostr.write((const char*) &fValue, sizeof(fValue));
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(int value)
    {
        if (_flipBytes)
        {
            int fValue = byte_order::flip_bytes(value);
            _ostr.write((const char*) &fValue, sizeof(fValue));
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(unsigned int value)
    {
        if (_flipBytes)
        {
            unsigned int fValue = byte_order::flip_bytes(value);
            _ostr.write((const char*) &fValue, sizeof(fValue));
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(long value)
    {
        if (_flipBytes)
        {
            if constexpr (sizeof(long) == 8)
            {
                long fValue = byte_order::flip_bytes((int64_t) value);
                _ostr.write((const char*) &fValue, sizeof(fValue));
            } else
            {
                long fValue = byte_order::flip_bytes((int32_t) value);
                _ostr.write((const char*) &fValue, sizeof(fValue));
            }
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(unsigned long value)
    {
        if (_flipBytes)
        {
            if constexpr (sizeof(long) == 8)
            {
                long fValue = byte_order::flip_bytes((uint64_t) value);
                _ostr.write((const char*) &fValue, sizeof(fValue));
            } else
            {
                long fValue = byte_order::flip_bytes((uint32_t) value);
                _ostr.write((const char*) &fValue, sizeof(fValue));
            }
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(float value)
    {
        if (_flipBytes)
        {
            const char* ptr = (const char*) &value;
            ptr += sizeof(value);
            for (unsigned i = 0; i < sizeof(value); ++i)
            {
                _ostr.write(--ptr, 1);
            }
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(double value)
    {
        if (_flipBytes)
        {
            const char* ptr = (const char*) &value;
            ptr += sizeof(value);
            for (unsigned i = 0; i < sizeof(value); ++i)
            {
                _ostr.write(--ptr, 1);
            }
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(long long value)
    {
        if (_flipBytes)
        {
            int64_t fValue = byte_order::flip_bytes(static_cast<int64_t>(value));
            _ostr.write((const char*) &fValue, sizeof(fValue));
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    binary_writer& binary_writer::operator<<(unsigned long long value)
    {
        if (_flipBytes)
        {
            uint64_t fValue = byte_order::flip_bytes(static_cast<uint64_t>(value));
            _ostr.write((const char*) &fValue, sizeof(fValue));
        } else
        {
            _ostr.write((const char*) &value, sizeof(value));
        }
        return *this;
    }

    void binary_writer::write_raw(const std::string& rawData)
    {
        _ostr.write(rawData.data(), (std::streamsize) rawData.length());
    }

    void binary_writer::write_raw(const char* buffer, std::streamsize length)
    {
        _ostr.write(buffer, length);
    }

    void binary_writer::flush()
    {
        _ostr.flush();
    }


}
