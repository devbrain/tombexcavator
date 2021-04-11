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
#include <tuple>

namespace formats::iff
{
    namespace detail
    {
        template<typename IffTraits>
        std::tuple<chunk_type, std::size_t> read_chunk_type(std::istream& is)
        {
            uint32_t dword;
            is.read((char*)&dword, sizeof(dword));
            if constexpr (IffTraits::chunk_id_big_endian())
            {
                return std::make_tuple(chunk_type{bsw::byte_order::from_big_endian(dword)}, sizeof(dword));
            }
            return std::make_tuple(chunk_type{bsw::byte_order::from_little_endian(dword)}, sizeof(dword));
        }
        // -------------------------------------------------------------------------------------------
        template<typename IffTraits>
        std::tuple<std::size_t, std::size_t> read_chunk_length(std::istream& is)
        {
            uint32_t dword;
            is.read((char*)&dword, sizeof(dword));

            if constexpr (IffTraits::chunk_length_big_endian())
            {
                return std::make_tuple(IffTraits::correct_size(bsw::byte_order::from_big_endian(dword)), sizeof(dword));
            }
            return std::make_tuple(IffTraits::correct_size(bsw::byte_order::from_little_endian(dword)), sizeof(dword));
        }
        // -------------------------------------------------------------------------------------------
        template<typename IffTraits>
        std::tuple<bool, std::size_t> read_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size, iff_events* events)
        {
            bool cont_parsing = events->on_start_group(is, type, offset, size);

            auto real_size = IffTraits::size_with_padding(size);
            std::size_t has_bytes = 0;
            if (cont_parsing)
            {
                while (has_bytes + 8 < real_size)
                {
                    auto [chunk_name, sz_type] = detail::read_chunk_type<IffTraits>(is);
                    auto [chunk_size, sz_size] = detail::read_chunk_length<IffTraits>(is);
                    has_bytes += sz_type;
                    has_bytes += sz_size;
                    if (IffTraits::is_group(chunk_name))
                    {
                        auto [rc, group_size] = read_group<IffTraits>(is, chunk_name, offset, chunk_size, events);
                        has_bytes += IffTraits::size_with_padding(group_size);
                        if (!rc)
                        {
                            return std::make_tuple(false, has_bytes);
                        }
                    } else
                    {
                        events->on_chunk(is, chunk_name, offset + has_bytes, chunk_size);
                        has_bytes += IffTraits::size_with_padding(chunk_size);
                        is.seekg(offset + has_bytes, std::ios::beg);
                    }
                }
            }
            events->on_end_group(type);

            is.seekg(offset + real_size, std::ios::beg);
            return std::make_tuple(cont_parsing, has_bytes);
        }
    }

    template<typename IffTraits>
    void iff_parser(std::istream& is, iff_events* events, uint64_t file_size)
    {
        uint64_t current = is.tellg();

        events->read_header(is, file_size);
        auto [chunk_name, nm_size] = detail::read_chunk_type<IffTraits>(is);
        auto [chunk_size, sz_size] = detail::read_chunk_length<IffTraits>(is);

        if constexpr (IffTraits::file_should_start_with_group())
        {
            if (!IffTraits::is_group(chunk_name))
            {
                RAISE_EX("File should start with group, ", chunk_name, " found");
            }
        }
        std::size_t has_bytes = nm_size + sz_size;
        while (has_bytes  < file_size)
        {
            if (IffTraits::is_group(chunk_name))
            {
                auto [rc, sz] = detail::read_group<IffTraits>(is, chunk_name, current, chunk_size, events);
                if (!rc)
                {
                    return;
                }
                has_bytes += sz;
            }
            else
            {
                events->on_chunk(is, chunk_name, current + has_bytes, chunk_size);
                has_bytes += IffTraits::size_with_padding(chunk_size);
                is.seekg(current + has_bytes, std::ios::beg);
            }
            if (has_bytes + 8 < file_size)
            {
                std::tie(chunk_name, nm_size) = detail::read_chunk_type<IffTraits>(is);
                std::tie(chunk_size, sz_size) = detail::read_chunk_length<IffTraits>(is);
                has_bytes = has_bytes + nm_size + sz_size;
            }
        }

    }
}

#endif //TOMBEXCAVATOR_IFF_EVENTS_HH
