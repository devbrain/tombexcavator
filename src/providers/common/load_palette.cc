//
// Created by igor on 29/03/2021.
//

#include <tomb-excavator/games/common/load_palette.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <istream>
#include <algorithm>

namespace games::common
{
    provider::dto::palette load_palette(const std::vector<char>& raw)
    {
        auto n = raw.size();
        if (n % 3 != 0)
        {
            RAISE_EX("Raw palette size should be divisible by 3");
        }
        if (n != 16*3 && n != 256*3)
        {
            RAISE_EX("Raw palette size should be 48 or 768 bytes");
        }
        bool is_8_bit = (std::find_if(raw.begin(), raw.end(), [](auto a) {return a > 63;}) != raw.end());
        return provider::dto::palette((unsigned char*)raw.data(), n, !is_8_bit);
    }

    provider::dto::palette load_palette(std::istream& is)
    {
        auto current = is.tellg();
        is.seekg(0, std::ios::end);
        auto end = is.tellg();
        is.seekg(current, std::ios::beg);
        std::size_t n = static_cast<std::size_t>(end - current);
        std::vector<char> raw(n);
        is.read(raw.data(), n);
        return load_palette(raw);
    }
}