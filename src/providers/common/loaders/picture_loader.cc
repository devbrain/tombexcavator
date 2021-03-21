//
// Created by igor on 10/03/2021.
//

#include <tomb-excavator/games/common/loaders/picture_loader.hh>
#include <tomb-excavator/formats/image/picture.hh>
namespace games::common
{

    picture_loader::picture_loader(std::string virt_name, std::string phys_name)
    : typed_single_entry_data_loader<formats::image::picture>(std::move(virt_name), std::move(phys_name))
    {
    }

    formats::image::picture picture_loader::load_content(std::istream& is) const
    {
        return formats::image::load_picture(is);
    }
}