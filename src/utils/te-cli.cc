//
// Created by igor on 22/02/2021.
//

#include <iostream>
#include <tomb-excavator/provider/vfs_registry.hh>

int main(int argc, char* argv[])
{
    provider::vfs_registry registry;
    provider::physfs::directory dir("/home/igor/games/CAVES/");
    auto fs = registry.get(dir);
    if (fs)
    {
        std::cout << fs->name() << std::endl;
    }
    else
    {
        std::cout << "No supported games found" << std::endl;
    }
    return 0;
}
