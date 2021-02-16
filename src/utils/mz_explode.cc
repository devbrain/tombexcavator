//
// Created by igor on 16/02/2021.
//

#include <formats/explode/mz/explode_exe_file.hh>
#include <mio/mmap.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "USAGE: " << argv[0] << " <input> <output>" << std::endl;
        return 1;
    }

    std::filesystem::path input = argv[1];
    std::filesystem::path output = argv[2];

    try
    {
        std::cout << "Reading " << input << std::endl;
        mio::mmap_source mmap(input.string());
        std::vector<char> out;
        if (!formats::explode::mz::explode_exe_file(mmap.data(), mmap.size(), out))
        {
            std::cout << "Unsupported format" << std::endl;
            return 1;
        }
        std::ofstream ofs (output, std::ios::binary | std::ios::out);
        ofs.write(out.data(), out.size());
        std::cout << "Exploded to " << output << std::endl;
    }
    catch (std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
