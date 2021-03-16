//
// Created by igor on 15/03/2021.
//

#include <sstream>
#include <tomb-excavator/formats/explode/mz/explode_exe_file.hh>
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
            case 3+7:
            case 3+8:
            case 3+14:
                return 23;
            default:
                return 24;
        }
    }
}
// ==========================================================================================
ccmap::ccmap(std::string phys_name)
: games::common::data_loader(std::move(phys_name))
{

}
// ------------------------------------------------------------------------------------------
void ccmap::open(std::shared_ptr<provider::physfs::directory> dir)
{
    auto istream = dir->open_file(physical_name());
    auto exe_reader = formats::explode::mz::explode_exe_file(*istream);
    exe_reader->stream().seekg(0x8CE0, std::ios::beg);
    for (int i=0; i<19; i++)
    {
        level_t lvl;
        auto rows = get_num_rows(i);
        for (std::size_t line = 0; line < rows; line++)
        {
            char x[40];
            char b;
            *exe_reader >> b;
            exe_reader->read_raw(x, 40);
            lvl.emplace_back(x);
        }
        m_levels.push_back(std::move(lvl));
    }
}
// ------------------------------------------------------------------------------------------
std::size_t ccmap::size() const
{
    return m_levels.size();
}
// ------------------------------------------------------------------------------------------
provider::file_type_t ccmap::load(std::size_t index) const
{
    std::ostringstream os;
    for (const auto& s : m_levels[index])
    {
        os << s << "\n";
    }
    return os.str();
}
// ------------------------------------------------------------------------------------------
std::string ccmap::name(std::size_t index) const
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
