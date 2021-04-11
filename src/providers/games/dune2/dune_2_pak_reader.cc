//
// Created by igor on 28/03/2021.
//

#include "dune_2_pak_reader.hh"
#include "emc.hh"

dune2_pak_reader::dune2_pak_reader(std::string phys_name)
: games::westwood::pak_loader(std::move(phys_name),
                              {games::common::make_entry_loader(emc::accept_build(), emc::load_build),
                               games::common::make_entry_loader(emc::accept_team(), emc::load_team),
                               games::common::make_entry_loader(emc::accept_unit(), emc::load_unit),
                                                     })
{

}

