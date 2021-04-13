//
// Created by igor on 03/04/2021.
//

#ifndef TOMBEXCAVATOR_WESTWOOD_ENTRY_LOADER_HH
#define TOMBEXCAVATOR_WESTWOOD_ENTRY_LOADER_HH

#include <tomb-excavator/export-games-westwood.h>
#include <tomb-excavator/games/common/loaders/archive_entry_loader.hh>

namespace games::westwood
{
    class GAMES_WESTWOOD_API westwood_entry_loader : public common::simple_loaders_registry
    {
    public:
        westwood_entry_loader ();
    };

}

#endif //TOMBEXCAVATOR_WESTWOOD_ENTRY_LOADER_HH
