//
// Created by igor on 10/03/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_LOADER_HH
#define TOMBEXCAVATOR_PICTURE_LOADER_HH

#include <tomb-excavator/games/common/loaders/data_loader.hh>

namespace games::common
{
    class GAMES_COMMON_API picture_loader : public single_entry_data_loader
    {
    public:
        picture_loader(std::string virt_name, std::string phys_name);
    private:
        provider::file_type_t read(std::istream& is) const override;
    };
}

#endif //TOMBEXCAVATOR_PICTURE_LOADER_HH
