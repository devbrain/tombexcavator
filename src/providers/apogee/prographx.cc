//
// Created by igor on 23/02/2021.
//

#include <array>

#include <tomb-excavator/games/apogee/prographx.hh>
#include <tomb-excavator/games/common/ega_palette.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/exceptions.hh>

#include "ega.hh"

namespace apogee
{
    common::tile_sheet load_prographx_tiles(std::istream& istream, std::size_t padding)
    {
        bsw::io::binary_reader reader(istream, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
        auto current = reader.stream().tellg();
        reader.stream().seekg(0, std::ios::end);
        auto fsize = reader.stream().tellg() - current;
        reader.stream().seekg(current, std::ios::beg);

        if (fsize < padding)
        {
            RAISE_EX("File size ", fsize, " is less than padding (", padding, ")");
        }
        detail::ega_byte_planar<false, 3, 1, 2, 4, 0> ega(reader);
        common::tile_sheet_builder builder;

        builder.add_palette(common::ega_palette());

        while (reader.stream().tellg() - current < fsize - padding)
        {
            unsigned char bcount;
            unsigned char bw;
            unsigned char bh;
            reader >> bcount >> bw >> bh;

            unsigned count = bcount & 0xFF;
            unsigned w = bw & 0xFF;
            unsigned h = bh & 0xFF;
            int py = 0;
            for (unsigned sprite = 0; sprite < count; sprite++)
            {
                for (unsigned y=0; y<h; y++)
                {
                    unsigned px = 0;
                    for (unsigned b=0; b<w; b++)
                    {
                        std::array<uint8_t, 8> pixels;
                        std::array<uint8_t, 8> mask;
                        ega.read_chunk(pixels, mask);
                        for (int i=0; i<8; i++)
                        {
                          //  auto [r,g,b]  = ega_colors[pixels[i]];
                          //  uint8_t a = mask[i] ? 0xFF : 0;
                          //  pic.put(px++, py, r, g, b, a);
                        }
                    }
                    py++;
                }
            }
        }

        return builder.build();
    }
}
