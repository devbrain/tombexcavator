//
// Created by igor on 28/03/2021.
//

#include "dune_2_pak_reader.hh"
#include "emc.hh"

dune2_pak_reader::dune2_pak_reader(std::string phys_name)
: games::westwood::pak_loader(std::move(phys_name),
                              {
    games::common::make_entry_loader(emc::accept_build(), emc::load_build),
    games::common::make_entry_loader(emc::accept_team(), emc::load_team),
    games::common::make_entry_loader(emc::accept_unit(), emc::load_unit),

    })
{

}

static constexpr int GLOBAL_PALETTE = 1;

void dune2_pak_reader::setup_loader_context(const std::string& entry_name,
                          fat_entry_t::props_map_t& props,
                          fat_entry_t::dependencies_t& deps)
{
    if (entry_name == "IBM.PAL")
    {
        props = fat_entry_t::props_map_t ({std::make_pair(GLOBAL_PALETTE, true)});
    }
    else
    {
        if (entry_name == "CHOAM.CPS")
        {
            deps.insert(GLOBAL_PALETTE);
        }
    }
}
