#include <vector>
#include <cstring>
#include <iostream>
#include "unlzexe.hh"
#include "exe_file.hh"
#include "struct_reader.hh"

static void build_rellocs_90(formats::input& file, std::vector<formats::explode::rellocation>& rellocs)
{
    formats::explode::byte_reader f(file);
    int16_t seg = 0;
    do
    {
        uint16_t t = f.word();
        int c = t & 0xFFFF;

        for (; c > 0; c--)
        {
            uint16_t offs = f.word();
            rellocs.push_back(formats::explode::rellocation(static_cast <uint16_t> (seg), offs));
        }
        seg = static_cast <int16_t> (seg + 0x1000);
    } // while (seg != static_cast <int16_t>(0xF000+0x1000));
    while (seg);
    //std::cout << "seg = " << std::hex << seg << std::endl;
}
// ----------------------------------------------------------------
static void build_rellocs_91(formats::input& file, std::vector<formats::explode::rellocation>& rellocs)
{
    int16_t seg = 0;
    int16_t offs = 0;
    int16_t span = 0;
    formats::explode::byte_reader f(file);
    while (true)
    {
        uint8_t s = f.byte();
        span = static_cast <int16_t> (static_cast <uint16_t> (s) & 0xFF);
        if (span == 0)
        {
            span = f.word();
            if (span == 0)
            {
                seg = static_cast <int16_t>(seg + 0x0FFF);
                continue;
            } else
            {
                if (span == 1)
                {
                    break;
                }
            }
        }
        offs = static_cast <int16_t>(offs + span);
        seg = static_cast <int16_t>(seg + static_cast <int16_t>((offs & ~0x0f) >> 4));
        offs &= 0x0f;
        rellocs.push_back(formats::explode::rellocation(static_cast <uint16_t> (seg), static_cast <uint16_t>(offs)));
    };
}

static uint32_t unpak_code(formats::explode::output_exe_file& oexe, formats::input& input, uint32_t offset)
{
    input.seek(offset);
    formats::explode::bit_reader bitstream(input);

    uint8_t data[0x4500], * p = data;
    std::size_t opos = 0;
    int16_t len = 0;
    int16_t span = 0;

    while (true)
    {
        if (p - data >= 0x4000)
        {
            oexe.code_put(opos, data, 0x2000);
            opos += 0x2000;
            p -= 0x2000;
            std::memmove(data, data + 0x2000, static_cast <std::size_t> (p - data));
        }
        if (bitstream.bit())
        {
            const uint8_t x = bitstream.byte();
            *p++ = x;
            continue;
        }

        if (!bitstream.bit())
        {
            len = static_cast <int16_t>(bitstream.bit() << 1);
            len = static_cast <int16_t>(len | bitstream.bit());
            len = static_cast <int16_t>(len + 2);
            span = static_cast <int16_t>((static_cast <uint16_t>(bitstream.byte()) & 0xFFFF) | 0xFF00);
        } else
        {
            span = static_cast<uint8_t>(static_cast<uint16_t> (bitstream.byte()) & 0xFFFF);
            len = static_cast <int16_t>(static_cast <uint16_t>(bitstream.byte()) & 0xFF);
            span = static_cast <int16_t>(span | static_cast<int16_t>(((len & ~0x07) << 5) | 0xe000));
            len = static_cast<int16_t>((len & 0x07) + 2);
            if (len == 2)
            {
                len = static_cast <int16_t> (static_cast <uint16_t> (bitstream.byte()) & 0xFF);
                if (len == 0)
                {
                    break;
                }
                if (len == 1)
                {
                    continue;
                } else
                {
                    len++;
                }
            }
        }
        for (; len > 0; len--, p++)
        {
            *p = *(p + span);
        }
    }
    if (p != data)
    {
        const std::size_t sz = static_cast <std::size_t> (p - data);
        oexe.code_put(opos, data, sz);
        opos += sz;
    }

    return static_cast <uint32_t> (opos);
}
// ----------------------------------------------------------------
namespace formats::explode
{
    bool unlzexe::accept(input_exe_file& inp)
    {
        static const uint8_t sig90[] =
                {
                        0x06, 0x0E, 0x1F, 0x8B, 0x0E, 0x0C, 0x00, 0x8B,
                        0xF1, 0x4E, 0x89, 0xF7, 0x8C, 0xDB, 0x03, 0x1E,
                        0x0A, 0x00, 0x8E, 0xC3, 0xB4, 0x00, 0x31, 0xED,
                        0xFD, 0xAC, 0x01, 0xC5, 0xAA, 0xE2, 0xFA, 0x8B,
                        0x16, 0x0E, 0x00, 0x8A, 0xC2, 0x29, 0xC5, 0x8A,
                        0xC6, 0x29, 0xC5, 0x39, 0xD5, 0x74, 0x0C, 0xBA,
                        0x91, 0x01, 0xB4, 0x09, 0xCD, 0x21, 0xB8, 0xFF,
                        0x4C, 0xCD, 0x21, 0x53, 0xB8, 0x53, 0x00, 0x50,
                        0xCB, 0x2E, 0x8B, 0x2E, 0x08, 0x00, 0x8C, 0xDA,
                        0x89, 0xE8, 0x3D, 0x00, 0x10, 0x76, 0x03, 0xB8,
                        0x00, 0x10, 0x29, 0xC5, 0x29, 0xC2, 0x29, 0xC3,
                        0x8E, 0xDA, 0x8E, 0xC3, 0xB1, 0x03, 0xD3, 0xE0,
                        0x89, 0xC1, 0xD1, 0xE0, 0x48, 0x48, 0x8B, 0xF0,
                        0x8B, 0xF8, 0xF3, 0xA5, 0x09, 0xED, 0x75, 0xD8,
                        0xFC, 0x8E, 0xC2, 0x8E, 0xDB, 0x31, 0xF6, 0x31,
                        0xFF, 0xBA, 0x10, 0x00, 0xAD, 0x89, 0xC5, 0xD1,
                        0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5, 0xB2,
                        0x10, 0x73, 0x03, 0xA4, 0xEB, 0xF1, 0x31, 0xC9,
                        0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
                        0xB2, 0x10, 0x72, 0x22, 0xD1, 0xED, 0x4A, 0x75,
                        0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0xD1, 0xD1,
                        0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
                        0xB2, 0x10, 0xD1, 0xD1, 0x41, 0x41, 0xAC, 0xB7,
                        0xFF, 0x8A, 0xD8, 0xE9, 0x13, 0x00, 0xAD, 0x8B,
                        0xD8, 0xB1, 0x03, 0xD2, 0xEF, 0x80, 0xCF, 0xE0,
                        0x80, 0xE4, 0x07, 0x74, 0x0C, 0x88, 0xE1, 0x41,
                        0x41, 0x26, 0x8A, 0x01, 0xAA, 0xE2, 0xFA, 0xEB,
                        0xA6, 0xAC, 0x08, 0xC0, 0x74, 0x40, 0x3C, 0x01,
                        0x74, 0x05, 0x88, 0xC1, 0x41, 0xEB, 0xEA, 0x89
                };

        static const uint8_t sig91[] =
                {
                        0x06, 0x0E, 0x1F, 0x8B, 0x0E, 0x0C, 0x00, 0x8B,
                        0xF1, 0x4E, 0x89, 0xF7, 0x8C, 0xDB, 0x03, 0x1E,
                        0x0A, 0x00, 0x8E, 0xC3, 0xFD, 0xF3, 0xA4, 0x53,
                        0xB8, 0x2B, 0x00, 0x50, 0xCB, 0x2E, 0x8B, 0x2E,
                        0x08, 0x00, 0x8C, 0xDA, 0x89, 0xE8, 0x3D, 0x00,
                        0x10, 0x76, 0x03, 0xB8, 0x00, 0x10, 0x29, 0xC5,
                        0x29, 0xC2, 0x29, 0xC3, 0x8E, 0xDA, 0x8E, 0xC3,
                        0xB1, 0x03, 0xD3, 0xE0, 0x89, 0xC1, 0xD1, 0xE0,
                        0x48, 0x48, 0x8B, 0xF0, 0x8B, 0xF8, 0xF3, 0xA5,
                        0x09, 0xED, 0x75, 0xD8, 0xFC, 0x8E, 0xC2, 0x8E,
                        0xDB, 0x31, 0xF6, 0x31, 0xFF, 0xBA, 0x10, 0x00,
                        0xAD, 0x89, 0xC5, 0xD1, 0xED, 0x4A, 0x75, 0x05,
                        0xAD, 0x89, 0xC5, 0xB2, 0x10, 0x73, 0x03, 0xA4,
                        0xEB, 0xF1, 0x31, 0xC9, 0xD1, 0xED, 0x4A, 0x75,
                        0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0x72, 0x22,
                        0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
                        0xB2, 0x10, 0xD1, 0xD1, 0xD1, 0xED, 0x4A, 0x75,
                        0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0xD1, 0xD1,
                        0x41, 0x41, 0xAC, 0xB7, 0xFF, 0x8A, 0xD8, 0xE9,
                        0x13, 0x00, 0xAD, 0x8B, 0xD8, 0xB1, 0x03, 0xD2,
                        0xEF, 0x80, 0xCF, 0xE0, 0x80, 0xE4, 0x07, 0x74,
                        0x0C, 0x88, 0xE1, 0x41, 0x41, 0x26, 0x8A, 0x01,
                        0xAA, 0xE2, 0xFA, 0xEB, 0xA6, 0xAC, 0x08, 0xC0,
                        0x74, 0x34, 0x3C, 0x01, 0x74, 0x05, 0x88, 0xC1,
                        0x41, 0xEB, 0xEA, 0x89, 0xFB, 0x83, 0xE7, 0x0F,
                        0x81, 0xC7, 0x00, 0x20, 0xB1, 0x04, 0xD3, 0xEB,
                        0x8C, 0xC0, 0x01, 0xD8, 0x2D, 0x00, 0x02, 0x8E,
                        0xC0, 0x89, 0xF3, 0x83, 0xE6, 0x0F, 0xD3, 0xEB,
                        0x8C, 0xD8, 0x01, 0xD8, 0x8E, 0xD8, 0xE9, 0x72
                };

        static const uint8_t sig91e[] = {
                0x50, 0x06, 0x0E, 0x1F, 0x8B, 0x0E, 0x0C, 0x00,
                0x8B, 0xF1, 0x4E, 0x89, 0xF7, 0x8C, 0xDB, 0x03,
                0x1E, 0x0A, 0x00, 0x8E, 0xC3, 0xFD, 0xF3, 0xA4,
                0x53, 0xB8, 0x2C, 0x00, 0x50, 0xCB, 0x2E, 0x8B,
                0x2E, 0x08, 0x00, 0x8C, 0xDA, 0x89, 0xE8, 0x3D,
                0x00, 0x10, 0x76, 0x03, 0xB8, 0x00, 0x10, 0x29,
                0xC5, 0x29, 0xC2, 0x29, 0xC3, 0x8E, 0xDA, 0x8E,
                0xC3, 0xB1, 0x03, 0xD3, 0xE0, 0x89, 0xC1, 0x48,
                0xD1, 0xE0, 0x8B, 0xF0, 0x8B, 0xF8, 0xF3, 0xA5,
                0x09, 0xED, 0x75, 0xD9, 0xFC, 0x8E, 0xC2, 0x8E,
                0xDB, 0x31, 0xF6, 0x31, 0xFF, 0xBA, 0x10, 0x00,
                0xAD, 0x89, 0xC5, 0xD1, 0xED, 0x4A, 0x75, 0x05,
                0xAD, 0x89, 0xC5, 0xB2, 0x10, 0x73, 0x03, 0xA4,
                0xEB, 0xF1, 0x31, 0xC9, 0xD1, 0xED, 0x4A, 0x75,
                0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0x72, 0x22,
                0xD1, 0xED, 0x4A, 0x75, 0x05, 0xAD, 0x89, 0xC5,
                0xB2, 0x10, 0xD1, 0xD1, 0xD1, 0xED, 0x4A, 0x75,
                0x05, 0xAD, 0x89, 0xC5, 0xB2, 0x10, 0xD1, 0xD1,
                0x41, 0x41, 0xAC, 0xB7, 0xFF, 0x8A, 0xD8, 0xE9,
                0x13, 0x00, 0xAD, 0x8B, 0xD8, 0xB1, 0x03, 0xD2,
                0xEF, 0x80, 0xCF, 0xE0, 0x80, 0xE4, 0x07, 0x74,
                0x0C, 0x88, 0xE1, 0x41, 0x41, 0x26, 0x8A, 0x01,
                0xAA, 0xE2, 0xFA, 0xEB, 0xA6, 0xAC, 0x08, 0xC0,
                0x74, 0x34, 0x3C, 0x01, 0x74, 0x05, 0x88, 0xC1,
                0x41, 0xEB, 0xEA, 0x89, 0xFB, 0x83, 0xE7, 0x0F,
                0x81, 0xC7, 0x00, 0x20, 0xB1, 0x04, 0xD3, 0xEB,
                0x8C, 0xC0, 0x01, 0xD8, 0x2D, 0x00, 0x02, 0x8E,
                0xC0, 0x89, 0xF3, 0x83, 0xE6, 0x0F, 0xD3, 0xEB,
                0x8C, 0xD8, 0x01, 0xD8, 0x8E, 0xD8, 0xE9, 0x72
        };

        uint8_t sigbuff[sizeof(sig90)];
        if ((inp[exe_file::RELLOC_OFFSET] != 0x1c) || (inp[exe_file::OVERLAY_NUM] != 0))
        {
            return false;
        }
        const std::size_t entry =
                (static_cast <uint32_t>(inp[exe_file::HEADER_SIZE_PARA] + inp[exe_file::INITIAL_CS]) << 4) +
                inp[exe_file::INITIAL_IP];
        input& m_file = inp.file();
        m_file.seek(entry);
        m_file.read_buff(reinterpret_cast <char*>(sigbuff), sizeof(sigbuff));

        if (std::memcmp(sigbuff, sig90, sizeof(sig90)) == 0)
        {
            return true;
        }
        if (std::memcmp(sigbuff, sig91, sizeof(sig91)) == 0)
        {
            return true;
        }
        if (std::memcmp(sigbuff, sig91e, sizeof(sig91e)) == 0)
        {
            return true;
        }
        return false;
    }
    // ===============================================================
    unlzexe::unlzexe(input_exe_file& inp)
            : m_file(inp.file()),
              m_exe_file(inp),
              m_ver(0)
    {
        static const std::size_t magic_offs = 2 * 0x0E;

        union
        {
            char bytes[4];
            uint32_t word;
        } magic;

        magic.word = 0;

        m_file.seek(magic_offs);
        m_file.read_buff(magic.bytes, 4);


        if (std::memcmp(magic.bytes, "LZ09", 4) == 0)
        {
            m_ver = 90;
        } else
        {
            if (std::memcmp(magic.bytes, "LZ91", 4) == 0)
            {
                m_ver = 91;
            } else
            {
                throw std::runtime_error("unlzexe - Unsuported version");
            }
        }
        const std::size_t header_pos = (inp[exe_file::HEADER_SIZE_PARA] + inp[exe_file::INITIAL_CS]) << 4;
        m_file.seek(header_pos);
        union
        {
            char* bytes;
            uint16_t* words;
        } u;
        u.words = m_header;
        m_file.read_buff(u.bytes, sizeof(m_header));

        for (int i = 0; i < eHEADER_MAX; i++)
        {
            m_header[i] = endian::read_le<uint16_t>(&m_header[i]);
        }

        if (m_ver == 90)
        {
            m_rellocs_offset = static_cast <uint32_t>(header_pos + 0x19D);
        } else
        {
            m_rellocs_offset = static_cast <uint32_t>(header_pos + 0x158);
        }
        m_code_offset = (static_cast <uint32_t>(inp[exe_file::INITIAL_CS]) -
                         static_cast <uint32_t>(m_header[eCOMPRESSED_SIZE]) +
                         static_cast <uint32_t>(inp[exe_file::HEADER_SIZE_PARA])) << 4;
    }
    // ------------------------------------------------------------------------
    void unlzexe::unpack(output_exe_file& oexe)
    {
        m_file.seek(m_rellocs_offset);
        if (m_ver == 90)
        {
            build_rellocs_90(m_file, oexe.rellocations());
        } else
        {
            build_rellocs_91(m_file, oexe.rellocations());
        }

        const uint32_t load_size = unpak_code(oexe, m_file, m_code_offset);

        for (int i = 0; i < exe_file::MAX_HEADER_VAL; i++)
        {
            const exe_file::header_t v = static_cast <exe_file::header_t> (i);
            oexe[v] = m_exe_file[v];
        }
        oexe[exe_file::INITIAL_IP] = m_header[eIP];
        oexe[exe_file::INITIAL_CS] = m_header[eCS];
        oexe[exe_file::INITIAL_SS] = m_header[eSS];
        oexe[exe_file::INITIAL_SP] = m_header[eSP];
        oexe[exe_file::RELLOC_OFFSET] = 0x1C;

        uint32_t fpos = static_cast <uint32_t>(0x1C + oexe.rellocations().size() * 4);
        uint32_t i = (0x200 - static_cast <int> (fpos)) & 0x1ff;
        oexe[exe_file::HEADER_SIZE_PARA] = static_cast <uint16_t>((fpos + i) >> 4);

        if (m_exe_file[exe_file::MAX_MEM_PARA] != 0)
        {
            int32_t delta = m_header[eINC_SIZE] + ((m_header[eDECOMPRESSOR_SIZE] + 16 - 1) >> 4) + 9;
            oexe[exe_file::MIN_MEM_PARA] = static_cast <uint16_t>(oexe[exe_file::MIN_MEM_PARA] - delta);
            if (m_exe_file[exe_file::MAX_MEM_PARA] != static_cast <uint16_t>(0xFFFF))
            {
                oexe[exe_file::MAX_MEM_PARA] = static_cast <uint16_t>(oexe[exe_file::MAX_MEM_PARA] -
                                                                      (m_header[eINC_SIZE] -
                                                                       oexe[exe_file::MIN_MEM_PARA]));
            }
        }

        oexe[exe_file::NUM_OF_BYTES_IN_LAST_PAGE] = static_cast <uint16_t> (
                (static_cast <uint16_t> (load_size) + (oexe[exe_file::HEADER_SIZE_PARA] << 4)) & 0x1ff);
        oexe[exe_file::NUM_OF_PAGES] = static_cast <uint16_t>(
                (load_size + (static_cast <uint32_t>(oexe[exe_file::HEADER_SIZE_PARA]) << 4) + 0x1ff) >> 9);


        oexe.eval_structures();
    }
    // ======================================================================
    uint32_t unlzexe::decomp_size() const
    {
        return 0;
    }
} // ns explode