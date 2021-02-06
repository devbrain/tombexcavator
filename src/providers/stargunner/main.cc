#include <iostream>
#include "stargunner.hh"
#include <algorithm>
#include <vfs/api/stdfilesystem.hh>
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
   // archive::file_input fi("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/end.spd");
    archive::file_input fi("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/starg11.spd");
    archive::file_input fi_anm("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/starg1.anm");
    archive::file_input fpal("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newdata/starg1.pal");

    auto pal = stargunner::load_palette(fpal);
    auto seqs = stargunner::load_animation_sequences(fi_anm);

    std::vector<std::string> names {"starg1", "starg11","starg12","starg13"};

    for (auto name : names)
    {
        std::string src ("/home/igor/proj/ares/tombexcavator/build/gcc/bin/out/output/newobjs/");
        src += name + ".spd";

        if (!stdfs::exists(name))
        {
            stdfs::create_directories(name);
        }

        archive::file_input inp(src.c_str());
        stargunner::dump_sprites(inp, pal, name);
    }


  //  auto tiles = stargunner::load_tile_sheet(seqs, pal, fi);
/*
    if (!tiles.save_to_png("zopa4.png"))
    {
        std::cout << "failed to save tiles" << std::endl;
    }
*/
    return 0;
}

