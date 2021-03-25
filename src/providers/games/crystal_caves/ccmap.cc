//
// Created by igor on 15/03/2021.
//

#include <sstream>
#include <tomb-excavator/formats/explode/mz/explode_exe_file.hh>
#include <tomb-excavator/provider/dto/map/map.hh>

#include "ccmap.hh"

namespace
{
    std::size_t get_num_rows(int level)
    {
        switch (level)
        {
            case 0: // intro
                return 5;
            case 1: // finale
                return 6;
            case 2: // main map
                return 25;
            case 3 + 7:
            case 3 + 8:
            case 3 + 14:
                return 23;
            default:
                return 24;
        }
    }

    std::string level_name(std::size_t index)
    {
        if (index == 0)
        {
            return "intro";
        }
        if (index == 1)
        {
            return "finale";
        }
        if (index == 2)
        {
            return "main_map";
        }

        std::ostringstream os;
        os << "level-" << index - 2;
        return os.str();
    }
} // anon. ns
// ==========================================================================================
constexpr int PROPS_IO = 0;
constexpr int PROPS_ROWS = 1;
constexpr int LEVEL_TYPE = 0;

static provider::file_content_t level_as_string ([[maybe_unused]] std::istream& is,
                                                 const games::common::archive_data_loader::fat_entry::file_info& fi,
                                                 const games::common::archive_data_loader::fat_entry::fat_entry::props_map_t& props)
{

    auto exe_reader = std::any_cast<bsw::io::binary_reader*>(props.find(PROPS_IO)->second);
    auto rows = std::any_cast<int>(props.find(PROPS_ROWS)->second);

    exe_reader->stream().seekg(fi.offset, std::ios::beg);
    std::ostringstream os;
    for (int i=0; i<rows; i++)
    {
        char line [42];
        exe_reader->read_raw(line, 41);
        line[41] = 0;
        os << std::string(line+1) << "\n";
    }
    return os.str();
}

ccmap::loaders_map_t ccmap::loaders()
{
    return {{LEVEL_TYPE, level_as_string}};
}

ccmap::ccmap(std::string phys_name)
        : games::common::archive_data_loader(std::move(phys_name), loaders())
{

}
// ------------------------------------------------------------------------------------------
std::vector<ccmap::fat_entry> ccmap::load_fat(std::istream& is)
{
    auto exe_reader = formats::explode::mz::explode_exe_file(is);
    m_exploded_exe = std::move(exe_reader);
    constexpr std::size_t MAPS_OFFSET = 0x8CE0;

    std::vector<ccmap::fat_entry> result;
    uint64_t offset = MAPS_OFFSET;
    for (int i = 0; i < 19; i++)
    {
        int rows = get_num_rows(i);
        fat_entry::props_map_t props { {PROPS_IO, m_exploded_exe.get()},
                                       {PROPS_ROWS, rows}};
        std::size_t size = 41*rows;

        fat_entry::file_info fi {41, offset, LEVEL_TYPE, provider::make_file_type<std::string>()};
        result.emplace_back(level_name(i), fi, props);
        offset += size;
    }
    return result;
}

