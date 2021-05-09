//
// Created by igor on 03/04/2021.
//

#include <tomb-excavator/games/westwood/westwood_entry_loader.hh>
#include <tomb-excavator/games/westwood/westwood_cps.hh>
#include <tomb-excavator/games/westwood/westwood_shp.hh>
#include <tomb-excavator/games/westwood/westwood_deps.hh>
#include <tomb-excavator/games/common/load_palette.hh>
#include <tomb-excavator/bsw/type_name/type_name.hpp>

namespace games::westwood
{
    template <typename T>
    std::optional<T> get_dependency(const common::loader_context_t& ctx, int key, const char* key_name)
    {
        const auto& [props, deps] = ctx;

        if (!props.exists(key))
        {
            return std::nullopt;
        }
        else
        {
            if (props.has_type<int>(key))
            {
                auto deps_key = props.get<int>(key);
                auto itr = deps.find(deps_key);
                if (itr == deps.end())
                {
                    RAISE_EX("Can not find dependency key ", key_name);
                }
                if (auto pal = std::get_if<provider::dto::palette>(itr->second.get()))
                {
                    return pal;
                } else
                {
                    RAISE_EX(key_name, " dependency expects ", type_name_v<T>, " type");
                }
            }
            else
            {
                RAISE_EX(key_name, " property key should be integer");
            }
        }
    }
    // ====================================================================================================
    static provider::dto::sprite_group shp_file_loader(std::istream& is,
                                                       uint64_t offs,
                                                       std::size_t size,
                                                       const common::loader_context_t& ctx)
    {
        auto pal = GET_ENTRY_DEPENDENCY(ctx, provider::dto::palette, SHP_PALETTE);
        is.seekg(offs, std::ios::beg);
        return load_shp(is, size, *pal);
    }
    // --------------------------------------------------------------------------------------------------
    static provider::dto::palette palette_file_loader(std::istream& is,
                                                 uint64_t offs, std::size_t size)
    {
        return games::common::load_palette(common::simple_loaders_registry::load_as_vector(is, offs, size));
    }
    // --------------------------------------------------------------------------------------------------
    static provider::dto::picture cps_file_loader(std::istream& is,
                                                  uint64_t offs, std::size_t size, const common::loader_context_t& ctx)
    {
        is.seekg(offs, std::ios::beg);

        const auto& [props, deps] = ctx;

        if (!props.exists(CPS_PALETTE))
        {
            return load_cps(is, size, std::nullopt);
        }
        else
        {
            if (props.has_type<int>(CPS_PALETTE))
            {
                auto deps_key = props.get<int>(CPS_PALETTE);
                auto itr = deps.find(deps_key);
                if (itr == deps.end())
                {
                    RAISE_EX("Can not find dependency key CPS_PALETTE");
                }
                if (auto pal = std::get_if<provider::dto::palette>(itr->second.get()))
                {
                    return load_cps(is, size, *pal);
                } else
                {
                    RAISE_EX("CPS_PALETTE dependency expects provider::dto::palette type");
                }
            }
            else
            {
                RAISE_EX("CPS_PALETTE property key should be integer");
            }
        }
    }
    // ==================================================================================================
    westwood_entry_loader::westwood_entry_loader()
        : common::simple_loaders_registry({
            common::make_entry_loader(by_ext(".pal"), palette_file_loader),
            common::make_entry_loader(by_ext(".cps"), cps_file_loader),
            common::make_entry_loader(by_ext(".shp"), shp_file_loader)
        })
    {

    }
}