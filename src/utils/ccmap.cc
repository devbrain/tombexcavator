//
// Created by igor on 22/03/2021.
//

#include <fstream>
#include <tomb-excavator/games/apogee/prographx.hh>
#include <tomb-excavator/export/sprite_group_export.hh>

int main(int argc, char* argv[])
{
    std::ifstream ifs("/home/igor/games/CAVES/CC1.GFX", std::ios::binary | std::ios::in);
    auto sg = apogee::load_prographx_tiles(ifs, 0, 0);
    exporter::to_png_with_ids(sg, "/tmp/cc.png");
    return 0;
}