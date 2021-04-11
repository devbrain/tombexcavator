//
// Created by igor on 11/04/2021.
//

#include <cstdint>

#include "emc.hh"
#include "chunked_file.hh"
#include <tomb-excavator/formats/iff/ea/ea_events.hh>
#include <tomb-excavator/formats/iff/ea/ea.hh>

struct emc_script
{
    explicit emc_script(emc::type_t t)
    : type(t) {}

    emc::type_t type;
    std::vector<uint16_t> offsets;
    std::vector<char> data;
};

// =============================================================================================================
games::common::archive_entry_loader::name_acceptor_t emc::accept_team()
{
    return games::westwood::westwood_entry_loader::by_name("team.emc");
}
// -----------------------------------------------------------------------------------------------------------
games::common::archive_entry_loader::name_acceptor_t emc::accept_build()
{
    return games::westwood::westwood_entry_loader::by_name("build.emc");
}
// -----------------------------------------------------------------------------------------------------------
games::common::archive_entry_loader::name_acceptor_t emc::accept_unit()
{
    return games::westwood::westwood_entry_loader::by_name("unit.emc");
}
// -----------------------------------------------------------------------------------------------------------
std::string emc::load_team(std::istream& is,
                             uint64_t offs, std::size_t size,
                             [[maybe_unused]] const games::common::fat_entry::props_map_t& props)
{
    return load(is, offs, size, eTEAM);
}
// -------------------------------------------------------------------------------------------------------------------
std::string emc::load_build(std::istream& is,
                            uint64_t offs, std::size_t size,
                            [[maybe_unused]] const games::common::fat_entry::props_map_t& props)
{
    return load(is, offs, size, eBUILD);
}
// -------------------------------------------------------------------------------------------------------------------
std::string emc::load_unit(std::istream& is,
                             uint64_t offs, std::size_t size,
                           [[maybe_unused]] const games::common::fat_entry::props_map_t& props)
{
    return load(is, offs, size, eUNIT);
}
// -------------------------------------------------------------------------------------------------------------------
std::string emc::load(std::istream& is,
                      uint64_t offs, std::size_t size,
                      type_t t)
{
    constexpr static auto ORDR = formats::iff::fourcc("ORDR");
    constexpr static auto DATA = formats::iff::fourcc("DATA");
    is.seekg(offs, std::ios::beg);
    chunked_file file(is, size);
    emc_script script (t);

    if (auto order_inf = file.find_section(ORDR); order_inf)
    {
        auto [offset, size] = *order_inf;
        if (size % 2 == 1)
        {
            RAISE_EX("Offsets size should be even");
        }
        size = size / 2;
        bsw::io::binary_reader rdr (is, bsw::io::binary_reader::BIG_ENDIAN_BYTE_ORDER);
        rdr.stream().seekg(offset, std::ios::beg);
        script.offsets.reserve(size);
        for (std::size_t i=0; i<size; i++)
        {
            uint16_t ofs;
            rdr >> ofs;
            script.offsets.push_back(ofs);
        }
    }
    else
    {
        RAISE_EX("Can not find offsets");
    }

    if (auto data_inf = file.find_section(DATA); data_inf)
    {
        auto [offset, size] = *data_inf;
        script.data.resize(size);
        is.seekg(offset, std::ios::beg);
        is.read(script.data.data(), size);
    }
    else
    {
        RAISE_EX("Can not find data");
    }
    return "";
}
