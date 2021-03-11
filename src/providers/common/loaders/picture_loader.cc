//
// Created by igor on 10/03/2021.
//

#include <tomb-excavator/games/common/loaders/picture_loader.hh>
#include <tomb-excavator/formats/image/picture.hh>
namespace games::common
{

    picture_loader::picture_loader(std::string virt_name, std::string phys_name)
    : data_loader(virt_name, phys_name)
    {
    }

    provider::file_type_t picture_loader::read(std::istream& is) const
    {
        return formats::image::load_picture(is);
    }
}