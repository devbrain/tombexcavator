//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_EXPLODE_EXE_FILE_HH
#define TOMBEXCAVATOR_EXPLODE_EXE_FILE_HH

#include <tomb-excavator/export-formats.h>
#include <tomb-excavator/bsw/io/binary_reader.hh>
#include <vector>
#include <memory>
#include <iosfwd>

namespace formats::explode::mz
{
    FORMATS_API bool explode_exe_file(const char* input, std::size_t input_size, std::vector<char>& output);
    FORMATS_API std::unique_ptr<bsw::io::binary_reader> explode_exe_file(std::istream& istream);
}

#endif //TOMBEXCAVATOR_EXPLODE_EXE_FILE_HH
