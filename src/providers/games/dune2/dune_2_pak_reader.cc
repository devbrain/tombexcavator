//
// Created by igor on 28/03/2021.
//

#include "dune_2_pak_reader.hh"

dune2_pak_reader::dune2_pak_reader(std::string phys_name)
: games::westwood::pak_loader(phys_name, {})
{

}

std::optional<dune2_pak_reader::file_data_t> dune2_pak_reader::get_file_data([[maybe_unused]] const std::string& name)
{
    return std::nullopt;
}