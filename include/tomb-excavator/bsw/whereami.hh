//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_WHEREAMI_HH
#define TOMBEXCAVATOR_WHEREAMI_HH

#include <tomb-excavator/bsw_export.h>
#include <filesystem>

namespace bsw
{
    BSW_API std::filesystem::path get_executable_path();
    BSW_API std::filesystem::path get_module_path();
}

#endif //TOMBEXCAVATOR_WHEREAMI_HH
