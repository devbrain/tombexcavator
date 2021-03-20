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
    // --------------------------------------------------------------------------------------
    std::pair<int, bool> map_tile(uint8_t map_tile)
    {
        switch (map_tile)
        {
            //Crystals (indices for CC1):
            case 0x52    :
                return {600, false};
            case 0x2B    :
                return {601, false};
            case 0x62    :
                return {602, false};
            case 0x63    :
                return {603, false};

                //Walls (indices for magenta walls):
            case 0x72    :
                return {1100, false};
            case 0x74    :
                return {1101, false};
            case 0x79    :
                return {1102, false};
            case 0x66    :
                return {1104, false};
            case 0x67    :
                return {1105, false};
            case 0x68    :
                return {1106, false};
            case 0x34    :
                return {1108, false};
            case 0x35    :
                return {1109, false};
            case 0x36    :
                return {1110, false};

                //Metal beams (indices for blue beams):
            case 0x44    :
                return {953, false};
            case 0x64    :
                return {954, false};
            case 0x98    :
                return {953, true};   //with hidden crystal
            case 0x99    :
                return {954, true};    //with hidden crystal
            case 0x9A    :
                return {955, true};   //with hidden crystal

                //Small platform (blue index):
            case 0x5F    :
                return {950, false};

                //Other Stuff:
            case 0x21    :
                return {650, false};
            case 0x22    :
                return {630, false};
            case 0x23    :
                return {124, false};
            case 0x24    :
                return {860, false};
            case 0x25    :
                return {539, false};
            case 0x26    :
                return {662, false};
            case 0x28    :
                return {184, false};
            case 0x29    :
                return {185, false};
            case 0x2A    :
                return {105, false};
            case 0x2C    :
                return {537, false};
            case 0x2D    :
                return {536, false};
            case 0x2E    :
                return {538, false};
            case 0x2F    :
                return {480, false};
            case 0x30    :
                return {43, false};
            case 0x38    :
                return {34, false};
            case 0x39    :
                return {96, false};
            case 0x3A    :
                return {629, false};
            case 0x3D    :
                return {689, false};
            case 0x3F    :
                return {50, false};
            case 0x41    :
                return {882, false};
            case 0x42    :
                return {6, false};
            case 0x45    :
                return {680, false};
            case 0x46    :
                return {470, false};
            case 0x47    :
                return {298, false};
            case 0x48    :
                return {590, false};
            case 0x49    :
                return {233, false};
            case 0x4A    :
                return {453, false};
            case 0x4B    :
                return {1050, false};
            case 0x4C    :
                return {1051, false};
            case 0x4D    :
                return {300, false};
            case 0x53    :
                return {154, false};
            case 0x56   :
            case 0xD7   :
            case 0xD6    :
                return {594, false};
            case 0x57    :
                return {150, false};
            case 0x58    :
                return {562, false};
            case 0x59    :
                return {250, false};
            case 0x5D    :
                return {299, false};
            case 0x5E    :
                return {201, false};
            case 0x61   :
            case 0x71   :
            case 0x82   :
                return {560, false};
            case 0x69    :
                return {674, false};
            case 0x6A    :
                return {605, false};
            case 0x6B    :
                return {1052, false};
            case 0x6C    :
                return {1053, false};
            case 0x6F    :
                return {100, false};
            case 0x70    :
                return {604, false};
            case 0x73   :
            case 0x77   :
            case 0x84    :
                return {559, false};
            case 0x76    :
                return {580, false};
            case 0x78    :
                return {12, false};
            case 0x7C    :
                return {184, false};
            case 0x7E    :
                return {212, false};
            case 0x85   :
                return {427, false};
            case 0x86   :
                return {422, false};
            case 0x87   :
                return {0, false};
            case 0x88   :
                return {1, false};
            case 0x89    :
                return {558, false};
            case 0x8A    :
                return {554, false};
            case 0x8B    :
                return {3, false};
            case 0x8C    :
                return {587, false};
            case 0x90    :
                return {386, false};
            case 0x91    :
                return {499, false};
            case 0x92    :
                return {476, false};
            case 0x93    :
                return {477, false};
            case 0x94    :
                return {378, false};
            case 0x95    :
                return {379, false};
            case 0xA0    :
                return {416, false};
            case 0xA1    :
                return {420, false};
            case 0xA2    :
                return {418, false};
            case 0xA3    :
                return {424, false};
            case 0xA4    :
                return {426, false};
            case 0xA5    :
                return {425, false};
            case 0xA6    :
                return {414, false};
            case 0xA7    :
                return {649, false};
            case 0xA8    :
                return {643, false};
            case 0xA9    :
                return {646, false};
            case 0xAA    :
                return {648, false};
            case 0xAB    :
                return {644, false};
            case 0xAC    :
                return {645, false};
            case 0xB0    :
                return {2, false};
            case 0xB1    :
                return {598, false};
            case 0xB2    :
                return {586, false};
            case 0xB3    :
                return {856, false};
            case 0xBA    :
                return {583, false};
            case 0xBB    :
                return {588, false};
            case 0xBC    :
                return {589, false};
            case 0xBD    :
                return {579, false};
            case 0xBE    :
                return {578, false};
            case 0xBF    :
                return {540, false};
            case 0xC0    :
                return {544, false};
            case 0xC1    :
                return {546, false};
            case 0xC2    :
                return {547, false};
            case 0xC3    :
                return {1057, false};
            case 0xC4    :
                return {1061, false};
            case 0xC6    :
                return {442, false};
            case 0xCA    :
                return {584, false};
            case 0xCB    :
                return {582, false};
            case 0xC7    :
                return {443, false};
            case 0xCD    :
                return {590, false};
            case 0xCE    :
                return {1056, false};
            case 0xCF    :
                return {543, false};
            case 0xD0    :
                return {557, false};
            case 0xD1    :
                return {542, false};
            case 0xD5    :
                return {639, false};
            case 0xD8    :
                return {581, false};
            case 0xD9    :
                return {545, false};
            case 0xDA    :
                return {541, false};
            case 0xE7    :
                return {548, false};
            case 0xE8    :
                return {394, false};
            case 0xE9    :
                return {395, false};
            case 0xEA    :
                return {396, false};
            case 0xEB    :
                return {397, false};
            case 0xEC    :
                return {398, false};
            case 0xED    :
                return {399, false};
            case 0xF0    :
                return {852, false};
            case 0xF3    :
                return {1044, false};
            case 0xF4    :
                return {8, false};
            case 0xF5    :
                return {10, false};
            case 0xF6    :
                return {182, false};
            case 0xF7    :
                return {183, false};
            case 0xF9    :
                return {553, false};
            case 0xFA    :
                return {561, false};
            case 0xFB    :
                return {585, false};
            case 0xFC    :
                return {498, false};
            case 0xFD    :
                return {499, false};
            case 0xFE    :
                return {476, false};
            default:
                return {-1, false};
        }
    }
    // --------------------------------------------------------------------------------------
    provider::dto::basic_map create_map(const std::vector<std::string>& level)
    {
        auto h = level.size();
        auto w = level.front().size();
        provider::dto::basic_map result(w, h);
        auto layer = result.add_layer();

        for (std::size_t y = 0; y < h; y++)
        {
            const auto& line = level[y];
            for (std::size_t x = 0; x < w; x++)
            {
                uint8_t tl = line[x];
                auto [id, has_hidden] = map_tile(tl);
                if (has_hidden)
                {
                    layer.put_tile((int)x, (int)y, id, {{"hidden_crystal", "true"}});
                }
                else
                {
                    if (id != -1)
                    {
                        layer.put_tile((int) x, (int) y, id, {});
                    }
                    else
                    {
                        layer.put_tile((int) x, (int) y, 0, {});
                    }
                }
            }
        }

        return result;
    }
} // anon. ns
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
    for (int i = 0; i < 19; i++)
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
