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
    provider::dto::sprite_group load_prographx_tiles(std::istream& istream, std::size_t padding, int first_sprite_id)
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

        provider::dto::sprite_group sg;
        sg.pal() = games::common::ega_palette();

        int sprite_id = first_sprite_id;
        const auto data_to_read = fsize - padding;
        while (!reader.stream().eof())
        {
            auto now_pos = reader.stream().tellg() - current;
            if (now_pos >= data_to_read)
            {
                break;
            }

            unsigned char bcount;
            unsigned char bw;
            unsigned char bh;
            reader >> bcount >> bw >> bh;

            unsigned count = bcount & 0xFF;
            unsigned w = bw & 0xFF;
            unsigned h = bh & 0xFF;

            for (unsigned sprite = 0; sprite < count; sprite++)
            {
                auto& sp = sg.add(8*w, h, true);
                sp.set_id(sprite_id++);
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
                            sp.get_canvas().put(8*b + i, y, pixels[i]);
                            if (mask[i])
                            {
                                sp.get_bitmask()->set(8 * b + i, y);
                            }
                        }
                    }
                }
            }
        }
        return sg;
    }
}
