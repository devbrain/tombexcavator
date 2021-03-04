//
// Created by igor on 04/03/2021.
//

#include <tomb-excavator/bsw/io/base64.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include "ios_init.hh"

namespace bsw::io
{

    namespace
    {
        const unsigned char OUT_ENCODING[64] =
                {
                        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                        '4', '5', '6', '7', '8', '9', '+', '/'
                };

        const unsigned char OUT_ENCODING_URL[64] =
                {
                        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                        '4', '5', '6', '7', '8', '9', '-', '_'
                };

    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder_buf::base64_encoder_buf(std::ostream& ostr, int options)
            :
            m_options(options),
            m_group_length(0),
            m_pos(0),
            m_line_length((options & BASE64_URL_ENCODING) ? 0 : 72),
            m_buf(*ostr.rdbuf()),
            m_out_enc((options & BASE64_URL_ENCODING) ? OUT_ENCODING_URL : OUT_ENCODING)
    {
    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder_buf::~base64_encoder_buf()
    {
        try
        {
            close();
        }
        catch (...)
        {
        }
    }
    // ---------------------------------------------------------------------------------------------------
    void base64_encoder_buf::set_line_length(int lineLength)
    {
        m_line_length = lineLength;
    }
    // ---------------------------------------------------------------------------------------------------
    int base64_encoder_buf::get_line_length() const
    {
        return m_line_length;
    }
    // ---------------------------------------------------------------------------------------------------
    int base64_encoder_buf::write_to_device(char c)
    {
        static const int eof = std::char_traits<char>::eof();

        m_group[m_group_length++] = (unsigned char) c;
        if (m_group_length == 3)
        {
            unsigned char idx;
            idx = m_group[0] >> 2;
            if (m_buf.sputc(m_out_enc[idx]) == eof)
            { return eof; }
            idx = ((m_group[0] & 0x03) << 4) | (m_group[1] >> 4);
            if (m_buf.sputc(m_out_enc[idx]) == eof)
            { return eof; }
            idx = ((m_group[1] & 0x0F) << 2) | (m_group[2] >> 6);
            if (m_buf.sputc(m_out_enc[idx]) == eof)
            { return eof; }
            idx = m_group[2] & 0x3F;
            if (m_buf.sputc(m_out_enc[idx]) == eof)
            { return eof; }
            m_pos += 4;
            if (m_line_length > 0 && m_pos >= m_line_length)
            {
                if (m_buf.sputc('\r') == eof)
                { return eof; }
                if (m_buf.sputc('\n') == eof)
                { return eof; }
                m_pos = 0;
            }
            m_group_length = 0;
        }
        return char_to_int(c);
    }
    // ---------------------------------------------------------------------------------------------------
    int base64_encoder_buf::close()
    {
        static const int eof = std::char_traits<char>::eof();

        if (sync() == eof)
        { return eof; }
        if (m_group_length == 1)
        {
            m_group[1] = 0;
            unsigned char idx;
            idx = m_group[0] >> 2;
            if (m_buf.sputc(m_out_enc[idx]) == eof)
            { return eof; }
            idx = ((m_group[0] & 0x03) << 4) | (m_group[1] >> 4);
            if (m_buf.sputc(m_out_enc[idx]) == eof)
            { return eof; }
            if (!(m_options & BASE64_NO_PADDING))
            {
                if (m_buf.sputc('=') == eof)
                { return eof; }
                if (m_buf.sputc('=') == eof)
                { return eof; }
            }
        } else
        {
            if (m_group_length == 2)
            {
                m_group[2] = 0;
                unsigned char idx;
                idx = m_group[0] >> 2;
                if (m_buf.sputc(m_out_enc[idx]) == eof)
                { return eof; }
                idx = ((m_group[0] & 0x03) << 4) | (m_group[1] >> 4);
                if (m_buf.sputc(m_out_enc[idx]) == eof)
                { return eof; }
                idx = ((m_group[1] & 0x0F) << 2) | (m_group[2] >> 6);
                if (m_buf.sputc(m_out_enc[idx]) == eof)
                { return eof; }
                if (!(m_options & BASE64_NO_PADDING))
                {
                    if (m_buf.sputc('=') == eof)
                    { return eof; }
                }
            }
        }
        m_group_length = 0;
        return m_buf.pubsync();
    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder_ios::base64_encoder_ios(std::ostream& ostr, int options)
            : m_buf(ostr, options)
    {
        te_ios_init(&m_buf);
    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder_ios::~base64_encoder_ios()
    {
    }
    // ---------------------------------------------------------------------------------------------------
    int base64_encoder_ios::close()
    {
        return m_buf.close();
    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder_buf* base64_encoder_ios::rdbuf()
    {
        return &m_buf;
    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder::base64_encoder(std::ostream& ostr, int options)
            : base64_encoder_ios(ostr, options), std::ostream(&m_buf)
    {
    }
    // ---------------------------------------------------------------------------------------------------
    base64_encoder::~base64_encoder()
    {
    }
    // ---------------------------------------------------------------------------------------------------
    namespace
    {
        unsigned char IN_ENCODING[256];
        bool IN_ENCODING_INIT = false;
        unsigned char IN_ENCODING_URL[256];
        bool IN_ENCODING_URL_INIT = false;

        struct static_initializer
        {
            static_initializer(int options)
            {
                if (options & BASE64_URL_ENCODING)
                {
                    if (!IN_ENCODING_URL_INIT)
                    {
                        for (unsigned i = 0; i < sizeof(IN_ENCODING_URL); i++)
                        {
                            IN_ENCODING_URL[i] = 0xFF;
                        }
                        for (unsigned i = 0; i < sizeof(OUT_ENCODING_URL); i++)
                        {
                            IN_ENCODING_URL[OUT_ENCODING_URL[i]] = static_cast<uint8_t>(i);
                        }
                        IN_ENCODING_URL[static_cast<unsigned char>('=')] = '\0';
                        IN_ENCODING_URL_INIT = true;
                    }
                } else
                {
                    if (!IN_ENCODING_INIT)
                    {
                        for (unsigned i = 0; i < sizeof(IN_ENCODING); i++)
                        {
                            IN_ENCODING[i] = 0xFF;
                        }
                        for (unsigned i = 0; i < sizeof(OUT_ENCODING); i++)
                        {
                            IN_ENCODING[OUT_ENCODING[i]] = static_cast<uint8_t>(i);
                        }
                        IN_ENCODING[static_cast<unsigned char>('=')] = '\0';
                        IN_ENCODING_INIT = true;
                    }
                }
            }
        };
    } // ns
    // ---------------------------------------------------------------------------------------------------
    base64_decoder_buf::base64_decoder_buf(std::istream& istr, int options)
            :
            m_options(options),
            m_group_length(0),
            m_group_index(0),
            m_buf(*istr.rdbuf()),
            m_in_enc((options & BASE64_URL_ENCODING) ? IN_ENCODING_URL : IN_ENCODING)
    {
        static static_initializer initializer_url(BASE64_URL_ENCODING);
        static static_initializer initializer(BASE64_NO_PADDING);
    }
    // ---------------------------------------------------------------------------------------------------
    base64_decoder_buf::~base64_decoder_buf() = default;
    // ---------------------------------------------------------------------------------------------------
    int base64_decoder_buf::read_from_device()
    {
        if (m_group_index < m_group_length)
        {
            return m_group[m_group_index++];
        } else
        {
            unsigned char buffer[4];
            int c;
            if ((c = read_one()) == -1)
            { return -1; }
            buffer[0] = static_cast<uint8_t>(c);
            if (m_in_enc[buffer[0]] == 0xFF) RAISE_EX("Bad base64 encoding");
            if ((c = read_one()) == -1)
            { return -1; }
            buffer[1] = static_cast<uint8_t>(c);
            if (m_in_enc[buffer[1]] == 0xFF) RAISE_EX("Bad base64 encoding");
            if (m_options & BASE64_NO_PADDING)
            {
                if ((c = read_one()) != -1)
                {
                    buffer[2] = static_cast<uint8_t>(c);
                } else
                {
                    buffer[2] = '=';
                }
                if (m_in_enc[buffer[2]] == 0xFF) RAISE_EX("Bad base64 encoding");
                if ((c = read_one()) != -1)
                {
                    buffer[3] = static_cast<uint8_t>(c);
                } else
                {
                    buffer[3] = '=';
                }
                if (m_in_enc[buffer[3]] == 0xFF) RAISE_EX("Bad base64 encoding");
            } else
            {
                if ((c = read_one()) == -1) RAISE_EX("Bad base64 encoding");
                buffer[2] = static_cast<uint8_t>(c);
                if (m_in_enc[buffer[2]] == 0xFF) RAISE_EX("Bad base64 encoding");
                if ((c = read_one()) == -1) RAISE_EX("Bad base64 encoding");
                buffer[3] = static_cast<uint8_t>(c);
                if (m_in_enc[buffer[3]] == 0xFF) RAISE_EX("Bad base64 encoding");
            }

            m_group[0] = (m_in_enc[buffer[0]] << 2) | (m_in_enc[buffer[1]] >> 4);
            m_group[1] = ((m_in_enc[buffer[1]] & 0x0F) << 4) | (m_in_enc[buffer[2]] >> 2);
            m_group[2] = (m_in_enc[buffer[2]] << 6) | m_in_enc[buffer[3]];

            if (buffer[2] == '=')
            {
                m_group_length = 1;
            } else
            {
                if (buffer[3] == '=')
                {
                    m_group_length = 2;
                } else
                {
                    m_group_length = 3;
                }
            }
            m_group_index = 1;
            return m_group[0];
        }
    }
    // ---------------------------------------------------------------------------------------------------
    int base64_decoder_buf::read_one()
    {
        int ch = m_buf.sbumpc();
        if (!(m_options & BASE64_URL_ENCODING))
        {
            while (ch == ' ' || ch == '\r' || ch == '\t' || ch == '\n')
            {
                ch = m_buf.sbumpc();
            }
        }
        return ch;
    }
    // ---------------------------------------------------------------------------------------------------
    base64_decoder_ios::base64_decoder_ios(std::istream& istr, int options)
            : m_buf(istr, options)
    {
        te_ios_init(&m_buf);
    }
    // ---------------------------------------------------------------------------------------------------
    base64_decoder_ios::~base64_decoder_ios()
    {
    }
    // ---------------------------------------------------------------------------------------------------
    base64_decoder_buf* base64_decoder_ios::rdbuf()
    {
        return &m_buf;
    }
    // ---------------------------------------------------------------------------------------------------
    base64_decoder::base64_decoder(std::istream& istr, int options)
            : base64_decoder_ios(istr, options), std::istream(&m_buf)
    {
    }
    // ---------------------------------------------------------------------------------------------------
    base64_decoder::~base64_decoder()
    {
    }
}