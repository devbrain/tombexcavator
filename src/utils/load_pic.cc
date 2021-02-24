//
// Created by igor on 16/02/2021.
//
#include <iostream>
#include "tomb-excavator/formats/image/picture.hh"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cout << "USAGE: " << argv[0] << " <IMAGE FILE>" << std::endl;
        return 1;
    }

    std::filesystem::path input = argv[1];
    auto output = input;
    auto fname = std::filesystem::current_path() / output.replace_extension(".png").filename();

    try
    {
        using namespace formats::image;
        std::cout << "Loading : " << input.string() << std::endl;
        auto pic = load_picture(input);
        if (!save_to_png(pic, fname))
        {
            std::cout << "Failed to save " << fname;
        }
        std::cout << "Saved : " << fname.string() << std::endl;
    } catch (std::exception& e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}