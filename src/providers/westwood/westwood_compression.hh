//
// Created by igor on 13/04/2021.
//

#ifndef TOMBEXCAVATOR_WESTWOOD_COMPRESSION_HH
#define TOMBEXCAVATOR_WESTWOOD_COMPRESSION_HH

#include <iosfwd>
#include <vector>
#include <cstdint>

namespace games::westwood
{
    void decompress_format_80(std::vector<uint8_t>& out, std::istream& is);
    void decompress_format_40(std::vector<uint8_t>& out, std::istream& is);
}

#endif //TOMBEXCAVATOR_WESTWOOD_COMPRESSION_HH
