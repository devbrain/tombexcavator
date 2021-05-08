//
// Created by igor on 15/04/2021.
//

#include <tomb-excavator/games/westwood/westwood_shp.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include "westwood_compression.hh"

namespace games::westwood
{
    struct frame_header
    {
        uint8_t slices;
        uint16_t width;
        uint8_t height;

        std::vector<uint8_t> remap_table;
    };

    // -----------------------------------------------------------------------------------------
    void build_sprite(provider::dto::sprite& sp, const frame_header& hdr,
                      const std::vector<uint8_t>& data)
    {
        std::size_t idx = 0;
        uint16_t x = 0;
        uint8_t y = 0;

        while (idx < data.size())
        {
            auto code = data[idx++];
            if (code == 0)
            {
                auto val = data[idx++];
                for (uint8_t i=0; i<val; i++)
                {
                    sp.get_bitmask()->set(x++, y);
                    if (x >= hdr.width)
                    {
                        x = 0;
                        y++;
                        if (y == hdr.height)
                        {
                            if (idx < data.size())
                            {
                                RAISE_EX("SHP sprite data is corrupted");
                            }
                            break;
                        }
                    }
                }
            }
            else
            {
                if (code < hdr.remap_table.size())
                {
                    code = hdr.remap_table[code];
                }
                sp.get_canvas().put(x++, y, code);
                if (x >= hdr.width)
                {
                    x = 0;
                    y++;
                    if (y == hdr.height)
                    {
                        if (idx < data.size())
                        {
                            RAISE_EX("SHP sprite data is corrupted");
                        }
                        break;
                    }
                }
            }
        }
        if (y < hdr.height)
        {
            RAISE_EX("SHP sprite data is corrupted");
        }
    }
    // -----------------------------------------------------------------------------------------

    provider::dto::sprite_group load_shp(std::istream& is, std::size_t entry_size,
                                         const provider::dto::palette& palette)
    {

        bsw::io::binary_reader rdr(is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
        auto start_pos = is.tellg();
        uint16_t num_of_frames;
        rdr >> num_of_frames;
        if (num_of_frames == 0)
        {
            RAISE_EX("SHP file is corrupted");
        }
        auto save_point = rdr.stream().tellg();
        uint16_t offs, offs2;
        rdr >> offs >> offs2;
        rdr.stream().seekg(save_point, std::ios::beg);
        std::vector<uint32_t> offsets;
        if (offs2)
        {
            // v1.0 format
            if (num_of_frames * sizeof(uint16_t) + sizeof(uint16_t) > entry_size)
            {
                RAISE_EX("SHP file is corrupted");
            }
            offsets.resize(num_of_frames + 1);
            for (uint16_t i = 0; i <= num_of_frames; i++)
            {
                rdr >> offs;
                offsets[i] = offs;
            }
        } else
        {
            // v1.7 format
            if (num_of_frames * sizeof(uint32_t) + sizeof(uint16_t) > entry_size)
            {
                RAISE_EX("SHP file is corrupted");
            }
            offsets.resize(num_of_frames + 1);
            auto start_offs = sizeof(num_of_frames);
            for (uint16_t i = 0; i <= num_of_frames; i++)
            {
                rdr >> offsets[i];
                offsets[i] += start_offs;
            }
        }
        if (offsets.back() != entry_size)
        {
            RAISE_EX("SHP file is corrupted");
        }
        provider::dto::sprite_group sg;
        sg.pal() = palette;

        for (uint16_t i = 0; i < num_of_frames; i++)
        {
            auto frame_start = (uint64_t) start_pos + offsets[i];
            rdr.stream().seekg(frame_start, std::ios::beg);
            frame_header hdr;
            uint16_t flags;
            uint16_t frame_size;
            uint16_t zero_compressed_size;
            rdr >> flags >> hdr.slices >> hdr.width >> hdr.height >> frame_size >> zero_compressed_size;
            auto next_offs = offsets[i] + frame_size;
            if (next_offs != offsets[i + 1])
            {
                RAISE_EX("SHP file is corrupted");
            }
            const bool has_remap = (flags & 0x1u) == 0x1u;
            const bool no_lcw = (flags & 0x2u) == 0x2u;
            const bool custom_size_remap = (flags & 0x4u) == 0x4u;
            if (!has_remap && custom_size_remap)
            {
                RAISE_EX("SHP file is corrupted");
            }
            if (has_remap)
            {
                uint8_t remap_size = 16;
                if (custom_size_remap)
                {
                    rdr >> remap_size;
                }
                hdr.remap_table.resize(remap_size);
                rdr.read_raw(hdr.remap_table.data(), hdr.remap_table.size());
            }
            const uint64_t header_end = rdr.stream().tellg();
            auto hdr_size = header_end - frame_start;
            auto data_size = frame_size - hdr_size;

            std::vector<uint8_t> frame_data;
            if (!no_lcw)
            {
                frame_data.resize(zero_compressed_size);
                decompress_format_80(frame_data, rdr.stream());
            } else
            {
                frame_data.resize(data_size);
                rdr.read_raw(frame_data.data(), frame_data.size());
            }
            build_sprite(sg.add(hdr.width, hdr.height, true), hdr, frame_data);
        }

        return sg;
    }
}