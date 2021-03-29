//
// Created by igor on 25/03/2021.
//

#include <tomb-excavator/games/westwood/westwood_pak_loader.hh>
#include <tomb-excavator/games/common/load_palette.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <tomb-excavator/bsw/string_utils.hh>

namespace games::westwood
{
    namespace detail
    {
        enum internal_file_t
        {
            WESTWOOD_BINARY_FILE = 0,
            WESTWOOD_PALETTE_FILE,
            WESTWOOD_KNOWN_LAST
        };

        std::vector<char> load_as_vector(std::istream& is, const pak_loader::fat_entry::file_info& fi)
        {
            std::vector<char> out(fi.size);

            const auto current = is.tellg();
            is.seekg(fi.offset, std::ios::beg);
            is.read(out.data(), out.size());
            is.seekg(current, std::ios::beg);
            return out;
        }

        provider::file_content_t binary_file_loader(std::istream& is, const pak_loader::fat_entry::file_info& fi,
                                                    [[maybe_unused]] const pak_loader::fat_entry::props_map_t& props)
        {
            return load_as_vector(is, fi);
        }

        provider::file_content_t palette_file_loader(std::istream& is, const pak_loader::fat_entry::file_info& fi,
                                                    [[maybe_unused]] const pak_loader::fat_entry::props_map_t& props)
        {
            return games::common::load_palette(load_as_vector(is, fi));
        }
    }
    // ---------------------------------------------------------------------------------------------------------
    pak_loader::loaders_map_t pak_loader::eval_loaders (const pak_loader::loaders_map_t& loaders)
    {
        pak_loader::loaders_map_t result;
        result[detail::WESTWOOD_BINARY_FILE] = detail::binary_file_loader;
        result[detail::WESTWOOD_PALETTE_FILE] = detail::palette_file_loader;

        for (const auto& [idx, ldr] : loaders)
        {
            result[detail::WESTWOOD_KNOWN_LAST + idx] = ldr;
        }

        return result;
    }
    // ---------------------------------------------------------------------------------------
    pak_loader::pak_loader(std::string phys_name, const loaders_map_t& loaders)
    : games::common::archive_data_loader(std::move(phys_name), eval_loaders(loaders))
    {

    }
    // ---------------------------------------------------------------------------------------
    std::vector<pak_loader::fat_entry> pak_loader::load_fat(std::istream& is)
    {
        const auto current = is.tellg();
        is.seekg(0, std::ios::end);
        const auto end = is.tellg();
        is.seekg(current, std::ios::beg);

        bsw::io::binary_reader reader(is, bsw::io::binary_reader::LITTLE_ENDIAN_BYTE_ORDER);
        using name_offset_t = std::tuple<std::string, uint32_t, uint32_t>;
        std::vector<name_offset_t> files;

        for (bool quit = false; !quit;)
        {
            uint32_t offs;
            reader >> offs;
            char ch;
            if (offs == 0 || static_cast<uint32_t>(current) + offs > end)
            {
                quit = true;
                if (!files.empty())
                {
                    auto& prev = files.back();
                    std::get<2>(prev) = static_cast<uint32_t>(end) - std::get<1>(prev);
                }
            }
            else
            {
                std::string name;
                do
                {
                    reader >> ch;
                    name += ch;
                }
                while (ch != 0);
                if (!files.empty())
                {
                    auto& prev = files.back();
                    uint32_t prev_size = offs - std::get<1>(prev);
                    std::get<2>(prev) = prev_size;
                }
                files.emplace_back(name, offs, 0);
            }
        }
        std::vector<pak_loader::fat_entry> result;
        for (std::size_t i=0; i<files.size(); i++)
        {
            common::archive_data_loader::fat_entry::props_map_t props;
            int internal_type = -1;
            const auto& [name, offs, size] = files[i];
            auto type = deduce_file_data(name, props, internal_type);
            common::archive_data_loader::fat_entry::file_info fi{size, offs, internal_type, type};
            result.emplace_back(name, fi, props);
        }

        return result;
    }
    // ---------------------------------------------------------------------------------------------------------

    template<typename T>
    std::pair<provider::file_type_t, int> make_ext(int interanl_type)
    {
        return {provider::make_file_type<T>(), interanl_type};
    }
    // ---------------------------------------------------------------------------------------------------------
    provider::file_type_t pak_loader::deduce_file_data(const std::string& name,
                                                       common::archive_data_loader::fat_entry::props_map_t& props,
                                                       int& internal_type)
    {
        auto lower_name = bsw::to_lower(name);
        auto dt = get_file_data(lower_name);
        if (dt)
        {
            props = std::get<1>(*dt);
            internal_type = std::get<2>(*dt);
            return std::get<0>(*dt);
        }

        static const std::map<std::string, std::pair<provider::file_type_t, int>> mappings = {
                {".pal", make_ext<provider::dto::palette>(detail::WESTWOOD_PALETTE_FILE)}
        };

        auto idx = lower_name.rfind('.');
        if (idx != std::string::npos)
        {
            auto ext = lower_name.substr(idx);
                auto itr = mappings.find(ext);
                if (itr != mappings.end())
                {
                    internal_type = itr->second.second;
                    return itr->second.first;
                }
        }

        internal_type = detail::WESTWOOD_BINARY_FILE;
        return provider::make_file_type<std::vector<char>>();
    }
}
