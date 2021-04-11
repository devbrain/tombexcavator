//
// Created by igor on 11/04/2021.
//

#ifndef TOMBEXCAVATOR_EMC_HH
#define TOMBEXCAVATOR_EMC_HH

#include <tomb-excavator/games/westwood/westwood_entry_loader.hh>

struct emc
{
    enum type_t
    {
        eBUILD,
        eTEAM,
        eUNIT
    };

    static games::common::archive_entry_loader::name_acceptor_t accept_team();
    static games::common::archive_entry_loader::name_acceptor_t accept_build();
    static games::common::archive_entry_loader::name_acceptor_t accept_unit();

    static std::string load_team(std::istream& is,
                            uint64_t offs, std::size_t size,
                            const games::common::fat_entry::props_map_t& props);
    static std::string load_build(std::istream& is,
                                 uint64_t offs, std::size_t size,
                                 const games::common::fat_entry::props_map_t& props);
    static std::string load_unit(std::istream& is,
                                 uint64_t offs, std::size_t size,
                                 const games::common::fat_entry::props_map_t& props);
private:
    static std::string load(std::istream& is, uint64_t offs, std::size_t size, type_t type);
};


#endif //TOMBEXCAVATOR_EMC_HH
