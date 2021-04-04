//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_EA_HH
#define TOMBEXCAVATOR_EA_HH

#include <tomb-excavator/formats/iff/fourcc.hh>

namespace formats::iff
{
    struct ea_iff
    {
        static constexpr bool chunk_length_big_endian() {return true;}
        static constexpr bool chunk_id_big_endian() {return true;}
        static constexpr bool file_should_start_with_group() {return true;}

        static bool is_group(const chunk_type& type)
        {
            static constexpr auto FORM = fourcc("FORM");
            static constexpr auto LIST = fourcc("LIST");
            static constexpr auto CAT  = fourcc("CAT ");
            return (type == FORM || type == LIST || type == CAT);
        }

        static std::size_t size_with_padding(std::size_t chunk_size)
        {
            if (chunk_size % 2)
            {
                return chunk_size + 1;
            }
            return chunk_size;
        }
    };
}

#endif //TOMBEXCAVATOR_EA_HH
