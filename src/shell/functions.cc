#include "functions.hh"
#include <iostream>
#include <vfs/api/system.hh>

void list_modules::function()
{
    std::cout << "zopa" << std::endl;
    for (const auto& m : vfs::get_modules())
    {
        std::cout << m.path() << "\t" << m.type() << std::endl;
    }
}

std::string list_modules::name()
{
    return "list_modules";
}