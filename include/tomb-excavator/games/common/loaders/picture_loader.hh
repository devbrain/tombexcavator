//
// Created by igor on 10/03/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_LOADER_HH
#define TOMBEXCAVATOR_PICTURE_LOADER_HH

#include <tomb-excavator/games/common/loaders/data_loader.hh>
#include <tomb-excavator/formats/image/picture.hh>

namespace games::common
{
    class GAMES_COMMON_API picture_loader : public typed_single_entry_data_loader<formats::image::picture>
    {
    public:
        picture_loader(std::string virt_name, std::string phys_name);
    private:
        formats::image::picture load_content(std::istream& is) const override;
    };
}

#endif //TOMBEXCAVATOR_PICTURE_LOADER_HH
