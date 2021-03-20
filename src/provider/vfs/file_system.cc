//
// Created by igor on 21/02/2021.
//

#include <tomb-excavator/provider/vfs/file_system.hh>
#include <vector>

namespace
{
    std::vector<std::string> split(const std::string& str, const std::string& delim)
    {
        std::vector<std::string> tokens;
        size_t prev = 0, pos = 0;
        do
        {
            pos = str.find(delim, prev);
            if (pos == std::string::npos) pos = str.length();
            std::string token = str.substr(prev, pos-prev);
            if (!token.empty()) tokens.push_back(token);
            prev = pos + delim.length();
        }
        while (pos < str.length() && prev < str.length());
        return tokens;
    }
} // ns

namespace provider::vfs
{
    file_system::file_system() = default;
    // ---------------------------------------------------------------------------
    file_system::~file_system() = default;
    // ---------------------------------------------------------------------------
    file_type_t file_system::open_file(const std::string& path)
    {

    }
}
