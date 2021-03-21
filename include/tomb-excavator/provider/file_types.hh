//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_FILE_TYPES_HH
#define TOMBEXCAVATOR_FILE_TYPES_HH

#include <variant>
#include <tuple>
#include <string>
#include <type_traits>

#include <tomb-excavator/provider/dto/video/canvas.hh>
#include <tomb-excavator/provider/dto/video/palette.hh>
#include <tomb-excavator/provider/dto/video/sprite.hh>
#include <tomb-excavator/provider/dto/video/picture.hh>
#include <tomb-excavator/provider/dto/map/map.hh>
#include <tomb-excavator/formats/image/picture.hh>

namespace provider
{
    namespace detail
    {
        template<typename ... Args>
        struct types_holder
        {
            using variant = std::variant<Args...>;
            using variant_ptr = std::variant<std::add_pointer_t<Args>...>;
        };

        using supported_types = types_holder<dto::sprite_group,
                dto::picture,
                formats::image::picture,
                dto::map,
                std::string>;
    }


    using file_content_t = detail::supported_types::variant;
    using file_type_t = detail::supported_types::variant_ptr;

    template <typename T>
    file_type_t make_file_type()
    {
        return (T*) nullptr;
    }
}

#endif //TOMBEXCAVATOR_FILE_TYPES_HH
