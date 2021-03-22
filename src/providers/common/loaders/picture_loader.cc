//
// Created by igor on 10/03/2021.
//

#include <tomb-excavator/games/common/loaders/picture_loader.hh>
#include <tomb-excavator/formats/image/picture.hh>
namespace games::common
{

    picture_loader::picture_loader(std::string virt_name, std::string phys_name)
    : single_entry_physical_data_loader<formats::image::picture>(std::move(virt_name), std::move(phys_name),
                                                                 load_content)
    {
    }
    // ----------------------------------------------------------------------------------------------
    formats::image::picture picture_loader::load_content(std::istream& is)
    {
        return formats::image::load_picture(is);
    }
}