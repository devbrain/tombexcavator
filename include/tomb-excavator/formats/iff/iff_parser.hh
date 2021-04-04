//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_IFF_PARSER_HH
#define TOMBEXCAVATOR_IFF_PARSER_HH

#include <tomb-excavator/export-formats.h>
#include <tomb-excavator/formats/iff/fourcc.hh>
#include <tomb-excavator/formats/iff/iff_events.hh>
#include <tomb-excavator/bsw/byte_order.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <istream>

namespace formats::iff
{
    namespace detail
    {
        template<typename IffTraits>
        chunk_type read_chunk_type(std::istream& is)
        {
            uint32_t dword;
            is.read((char*)&dword, sizeof(dword));
            if constexpr (IffTraits::chunk_id_big_endian())
            {
                return chunk_type{bsw::byte_order::from_big_endian(dword)};
            }
            return chunk_type{bsw::byte_order::from_little_endian(dword)};
        }
        // -------------------------------------------------------------------------------------------
        template<typename IffTraits>
        std::size_t read_chunk_length(std::istream& is)
        {
            uint32_t dword;
            is.read((char*)&dword, sizeof(dword));

            if constexpr (IffTraits::chunk_length_big_endian())
            {
                return bsw::byte_order::from_big_endian(dword);
            }
            return bsw::byte_order::from_little_endian(dword);
        }
        // -------------------------------------------------------------------------------------------
        template<typename IffTraits>
        void read_chunk(std::istream& is, chunk_type type, uint64_t offset, std::size_t size, iff_events* events)
        {
            events->on_chunk(is, type, offset, size);
            is.seekg(offset + IffTraits::size_with_padding(size), std::ios::beg);
        }
        // -------------------------------------------------------------------------------------------
        template<typename IffTraits>
        bool read_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size, iff_events* events)
        {
            bool cont_parsing = events->on_start_group(is, type, offset, size);

            auto end_offset = offset + IffTraits::size_with_padding(size);
            if (cont_parsing)
            {
                while (offset < end_offset)
                {
                    auto chunk_name = detail::read_chunk_type<IffTraits>(is);
                    auto chunk_size = detail::read_chunk_length<IffTraits>(is);
                    offset = is.tellg();
                    if (IffTraits::is_group(chunk_name))
                    {
                        if (!read_group<IffTraits>(is, chunk_name, offset, chunk_size, events))
                        {
                            return false;
                        }
                    } else
                    {
                        read_chunk<IffTraits>(is, chunk_name, offset, chunk_size, events);
                    }
                    offset = is.tellg();
                }
            }
            events->on_end_group(type);
            is.seekg(end_offset, std::ios::beg);
            return cont_parsing;
        }
    }

    template<typename IffTraits>
    void iff_parser(std::istream& is, iff_events* events)
    {
        uint64_t current = is.tellg();
        is.seekg(0, std::ios::end);
        const uint64_t file_size = is.tellg();
        is.seekg(current, std::ios::beg);

        events->read_header(is, file_size);
        auto chunk_name = detail::read_chunk_type<IffTraits>(is);
        auto chunk_size = detail::read_chunk_length<IffTraits>(is);

        if constexpr (IffTraits::file_should_start_with_group())
        {
            if (!IffTraits::is_group(chunk_name))
            {
                RAISE_EX("File should start with group, ", chunk_name, " found");
            }
        }

        while (current + 8 < file_size)
        {
            current = is.tellg();
            if (IffTraits::is_group(chunk_name))
            {
                if (!detail::read_group<IffTraits>(is, chunk_name, current, chunk_size, events))
                {
                    return;
                }
            }
            else
            {
                detail::read_chunk<IffTraits>(is, chunk_name, current, chunk_size, events);
            }
            current = is.tellg();
        }

    }
}

#endif //TOMBEXCAVATOR_IFF_EVENTS_HH
