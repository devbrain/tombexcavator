//
// Created by igor on 28/03/2021.
//

#ifndef TOMBEXCAVATOR_LOADER_FUNCTIONS_HH
#define TOMBEXCAVATOR_LOADER_FUNCTIONS_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/file_types.hh>

#include <string>
#include <iosfwd>
#include <functional>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::common
{
    struct GAMES_COMMON_API file_loader
    {
        using entry_loader_t = std::function<provider::file_content_t (std::istream&)>;

        provider::file_type_t type;
        entry_loader_t function;

        template<typename T>
        file_loader(std::function<T (std::istream&)> ldr)
        : type(provider::make_file_type<T>()),
          function([ldr] (std::istream& is) -> provider::file_content_t { return ldr(is);})
        {

        }

        file_loader() = delete;
        file_loader(const file_loader&) = default;
        file_loader& operator = (const file_loader&) = default;
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_LOADER_FUNCTIONS_HH
