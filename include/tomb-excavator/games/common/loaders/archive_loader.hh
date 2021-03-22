//
// Created by igor on 22/03/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_LOADER_HH
#define TOMBEXCAVATOR_ARCHIVE_LOADER_HH

#include <tomb-excavator/games/common/loaders/data_loader.hh>

namespace games::common
{
    class GAMES_COMMON_API archive_data_loader : public physical_data_loader
    {
    public:
        explicit archive_data_loader(std::string phys_name);

    };
}

#endif //TOMBEXCAVATOR_ARCHIVE_LOADER_HH
