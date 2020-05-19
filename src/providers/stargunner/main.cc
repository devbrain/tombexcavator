#include <iostream>
#include "stargunner.hh"
#include <stdio.h>
#include <archive/io.hh>

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
    archive::file_input fi("/tmp/star.anm");
    uint32_t sprites, area;
    fi >> sprites >> area;
    std::cout << "sprites " << sprites  << "/" << area << std::endl;
    for (uint32_t i = 0; i<sprites; i++)
    {
        uint16_t idx;
        uint16_t unk;
        fi >> idx >> unk;
        std::cout << idx << " : " << unk << std::endl;
    }

    std::cout << "TELL : " << fi.tell() << std::endl;
    std::cout << "REMAINS : " << fi.bytes_remains() << std::endl;
    return 0;
}

