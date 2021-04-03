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

static std::string level_as_string ([[maybe_unused]] std::istream& is,
                                                 uint64_t offset,
                                                 [[maybe_unused]] std::size_t size,
                                                 const games::common::base_archive_data_loader::fat_entry_t::fat_entry::props_map_t& props)
{

    auto exe_reader = props.get<bsw::io::binary_reader*>(PROPS_IO);
    auto rows = props.get<int>(PROPS_ROWS);

    exe_reader->stream().seekg(offset, std::ios::beg);
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


using namespace games::common;

class ccmap_entry_loaders : public archive_entry_loaders_registry
{
public:
    ccmap_entry_loaders()
    : archive_entry_loaders_registry({make_entry_loader(
            [](const std::string&) { return true; },
            level_as_string)})
    {

    }
};

ccmap::ccmap(std::string phys_name)
        : base_archive_data_loader(std::move(phys_name),
                                   std::make_unique<ccmap_entry_loaders>())
{

}
// ------------------------------------------------------------------------------------------
void ccmap::load_fat(std::istream& is, games::common::fat_events& builder)
{
    auto exe_reader = formats::explode::mz::explode_exe_file(is);
    m_exploded_exe = std::move(exe_reader);
    constexpr std::size_t MAPS_OFFSET = 0x8CE0;


    uint64_t offset = MAPS_OFFSET;
    for (int i = 0; i < 19; i++)
    {
        int rows = get_num_rows(i);
        fat_entry_t::props_map_t props { {PROPS_IO, m_exploded_exe.get()},
                                       {PROPS_ROWS, rows}};
        std::size_t size = 41*rows;
        fat_entry_t::file_info fi {41, offset, LEVEL_TYPE, provider::make_file_type<std::string>()};
        builder.add_file(level_name(i), fi, props);
        offset += size;
    }
}

