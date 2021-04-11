//
// Created by igor on 03/04/2021.
//

#include <tomb-excavator/games/westwood/westwood_entry_loader.hh>
#include <tomb-excavator/games/common/load_palette.hh>
#include <tomb-excavator/bsw/string_utils.hh>

namespace games::westwood
{
    static provider::dto::palette palette_file_loader(std::istream& is,
                                                 uint64_t offs, std::size_t size,
                                                 [[maybe_unused]] const common::fat_entry::props_map_t& props)
    {
        return games::common::load_palette(common::simple_loaders_registry::load_as_vector(is, offs, size));
    }
    // ==================================================================================================
    westwood_entry_loader::westwood_entry_loader()
        : common::simple_loaders_registry({common::make_entry_loader(by_ext(".pal"), palette_file_loader)})
    {

    }
    // ---------------------------------------------------------------------------------------------------
    common::archive_entry_loader::name_acceptor_t westwood_entry_loader::by_ext(const std::string& ext)
    {
        return [ext](const std::string& name) {
            auto lname = bsw::to_lower(name);
            auto idx = lname.rfind('.');
            if (idx == std::string::npos) {
                return false;
            }
            return (lname.substr(idx) == ext);
        };
    }
    // ---------------------------------------------------------------------------------------------------
    common::archive_entry_loader::name_acceptor_t westwood_entry_loader::by_name(const std::string& name)
    {
        return [name](const std::string& nm) {
            auto lname = bsw::to_lower(nm);
            return (lname == name);
        };
    }
}