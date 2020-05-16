#include "stargunner.hh"
#include <stdio.h>

int main(int argc, char* argv[])
{
    stargunner::dlt_archive dlt("/home/igor/games/Stargunner/data/STARGUN.DLT");

    auto e = dlt.get_entry("setup.pcx");
    std::vector<std::byte> out;
    dlt.decompress(e, out);
    FILE* f = fopen ("/tmp/setup0.pcx", "wb");
    fwrite(out.data(),1, out.size(), f);
    fclose(f);
    return 0;
}

