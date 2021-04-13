//
// Created by igor on 12/04/2021.
//

#include <tomb-excavator/games/westwood/westwood_cps.hh>

#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include "westwood_compression.hh"

namespace games::westwood
{
    struct cps_header
    {
        uint16_t file_size;
        uint16_t compression_type;
        uint32_t uncompressed_size;
        uint16_t palette_size;
    };

    provider::dto::picture load_cps(std::istream& is, std::size_t entry_size,
                                    const std::optional<provider::dto::palette>& default_palette)
    {
        if (entry_size < sizeof(cps_header))
        {
            RAISE_EX("CPS is corrupted");
        }
        bsw::io::binary_reader rd(is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);

        cps_header header{10, 10, 10, 10};
        rd >> header.file_size >> header.compression_type >> header.uncompressed_size >> header.palette_size;

        if (header.palette_size == 0 && !default_palette)
        {
            RAISE_EX("Can not load CPS without palette");
        }

        if (header.compression_type == 0 || header.compression_type == 4)
        {
            if (header.uncompressed_size != 64000)
            {
                RAISE_EX("CPS should hold 320x200 image data");
            }
        } else
        {
            RAISE_EX("Unknown compression format ", header.compression_type);
        }

        provider::dto::palette pal;
        if (header.palette_size > 0)
        {
            std::vector<unsigned char> pal_data(header.palette_size);
            rd.read_raw(pal_data.data(), pal_data.size());
            pal = provider::dto::palette(pal_data);
        }
        if (default_palette)
        {
            pal = *default_palette;
        }
        provider::dto::picture out(320, 200, pal);
        auto& pixels = out.get_canvas().pixels();
        if (header.compression_type == 0)
        {
            rd.stream().read((char*) pixels.data(), pixels.size());
        } else
        {
            decompress_format_80(pixels, rd.stream());
        }
        return out;
    }
}