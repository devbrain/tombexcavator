//
// Created by igor on 28/03/2021.
//

#ifndef TOMBEXCAVATOR_DUNE_2_PAK_READER_HH
#define TOMBEXCAVATOR_DUNE_2_PAK_READER_HH

#include <tomb-excavator/games/westwood/westwood_pak_loader.hh>

class dune2_pak_reader : public games::westwood::pak_loader
{
public:
    explicit dune2_pak_reader(std::string phys_name);
private:
    void setup_loader_context(const std::string& entry_name,
                              fat_entry_t::props_map_t& props,
                              fat_entry_t::dependencies_t& deps)  override;
};


#endif //TOMBEXCAVATOR_DUNE_2_PAK_READER_HH
