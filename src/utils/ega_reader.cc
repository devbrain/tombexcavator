//
// Created by igor on 19/02/2021.
//

#include <iostream>
#include <fstream>
#include <array>
#include <tuple>
#include <bsw/io/binary_reader.hh>
#include <formats/image/picture.hh>
#include "../providers/apogee/ega.hh"


std::array<std::tuple<uint8_t, uint8_t, uint8_t>, 16> ega_colors = {
        std::make_tuple(0x00,0x00,0xAA),
        std::make_tuple(0x00,0xAA,0x00),
        std::make_tuple(0x00,0xAA,0xAA),
        std::make_tuple(0xAA,0x00,0x00),
        std::make_tuple(0xAA,0x00,0xAA),
        std::make_tuple(0xAA,0x55,0x00),
        std::make_tuple(0xAA,0xAA,0xAA),
        std::make_tuple(0x55,0x55,0x55),
        std::make_tuple(0x55,0x55,0xFF),
        std::make_tuple(0x55,0xFF,0x55),
        std::make_tuple(0x55,0xFF,0xFF),
        std::make_tuple(0xFF,0x55,0x55),
        std::make_tuple(0xFF,0x55,0xFF),
        std::make_tuple(0xFF,0xFF,0x55),
        std::make_tuple(0xFF,0xFF,0xFF)
};


int main(int argc, char* argv[])
{
    std::ifstream ifs("/home/igor/games/CAVES/CC1.GFX", std::ios::in | std::ios::binary);

    bsw::io::binary_reader reader(ifs, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);

    unsigned char bcount;
    unsigned char bw;
    unsigned char bh;
    reader >> bcount >> bw >> bh;

    unsigned count = bcount & 0xFF;
    unsigned w = bw & 0xFF;
    unsigned h = bh & 0xFF;

    formats::image::picture pic(w*8, h*count, true);
    formats::image::detail::ega_byte_planar<false, 3, 1, 2, 4, 0> ega(reader);
    unsigned py = 0;


    auto pos = reader.stream().tellg();
    std::cout << pos << " : " << "count " << count << " " << w << "x" << h << std::endl;
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
                    auto [r,g,b]  = ega_colors[pixels[i]];
                    uint8_t a = mask[i] ? 0xFF : 0;
                    pic.put(px++, py, r, g, b, a);
                }
            }
            py++;
        }
    }
    formats::image::save_to_png(pic, "cc.png");
}
