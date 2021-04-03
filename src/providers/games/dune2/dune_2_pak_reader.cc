//
// Created by igor on 28/03/2021.
//

#include "dune_2_pak_reader.hh"

dune2_pak_reader::dune2_pak_reader(std::string phys_name)
: games::westwood::pak_loader(phys_name, {})
{

}

