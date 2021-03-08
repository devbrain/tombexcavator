//
// Created by igor on 08/03/2021.
//

#ifndef TOMBEXCAVATOR_FILE_TYPE_TO_STRING_HH
#define TOMBEXCAVATOR_FILE_TYPE_TO_STRING_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/provider/file_types.hh>
#include <string>

namespace exporter
{
    [[nodiscard]] EXPORT_API std::string file_type_to_string(const provider::file_type_t& ft);
}

#endif //TOMBEXCAVATOR_FILE_TYPE_TO_STRING_HH
