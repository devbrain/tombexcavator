#include <iostream>
#include "stargunner.hh"
#include <stdio.h>
#include <archive/io.hh>
#include <archive/output.hh>

int main(int argc, char* argv[])
{
    /*
    stargunner::dlt_archive dlt("/home/igor/games/Stargunner/data/STARGUN.DLT");

    auto e = dlt.get_entry("newobjs\\starg1.anm");
    std::vector<std::byte> out;
    dlt.decompress(e, out);
    FILE* f = fopen ("/tmp/star.anm", "wb");
    fwrite(out.data(),1, out.size(), f);
    fclose(f);
*/
    archive::file_input fi("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/end.spd");
    //archive::file_input fi("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/end.spd");
    archive::file_input fi_anm("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/end.anm");
    archive::file_input fpal("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newdata/starg1.pal");
    std::vector<unsigned char> pal(768);
    fpal.read_buff((char*)pal.data(), 768);

    archive::save_palette("zopa.png", pal.data(), 256);

    auto seqs = stargunner::load_animation_sequences(fi_anm);
    uint32_t sprites, area, read3 ;
    fi >> sprites >> area >> read3;
    std::cout << "sprites " << sprites  << "/" << area  << "/" << read3 << std::endl;

    std::vector<archive::pixels_frame> frames;
    std::size_t data_start = fi.tell() + read3*40;
    for (uint32_t i = 0; i<read3; i++)
    {
        char sprite_data [40];
        fi.read_buff(sprite_data, 40);
        archive::inmem_input spd_inp((unsigned char*)sprite_data, 40);
        uint32_t sprite_index; // 0
        uint32_t w; // 1
        uint32_t h; // 2
        int32_t grp_num; // 3
        uint32_t unk2; // 4
        int32_t unk3; // 5
        int32_t unk4; // 6
        uint32_t offset_start; // 7
        uint32_t unk5; // 8
        uint32_t offset_end; // 9
        spd_inp >> sprite_index >> w >> h >> grp_num >> unk2 >> unk3 >> unk4 >>  offset_start >> unk5 >> offset_end;
        bool found = false;
        for (auto x : seqs[0])
        {
            if (sprite_index == x)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            std::cout << sprite_index << " " << w << " " << h << "( " << " " << grp_num << " " << unk2 << ") "
                      << offset_start
                      << " | " << unk3 << "," << unk4 << "," << unk5 << "," << offset_end
                      << std::endl;
            archive::pixels_frame frame(w, h);
            auto now = fi.tell();
            fi.seek (data_start + offset_start);
            fi.read_buff((char*)frame.pixels.data(), frame.pixels.size());
            fi.seek (now);
            frames.push_back(frame);
        }
    }

    std::cout << "TELL : " << fi.tell() << std::endl;
    std::cout << "REMAINS : " << fi.bytes_remains() << std::endl;

    archive::save_pixels("frame.png", frames, pal);
    return 0;
}

