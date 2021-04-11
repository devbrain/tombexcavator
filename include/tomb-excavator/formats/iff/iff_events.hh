//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_IFF_EVENTS_HH
#define TOMBEXCAVATOR_IFF_EVENTS_HH

#include <tomb-excavator/export-formats.h>
#include <tomb-excavator/formats/iff/fourcc.hh>
#include <iosfwd>
#include <cstdint>
#include <variant>
#include <tuple>

namespace formats::iff
{
    class FORMATS_API iff_events
    {
    public:
        virtual ~iff_events();
        virtual void read_header(std::istream& is, uint64_t file_size);
        // return false to stop parsing
        [[nodiscard]] virtual bool on_start_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) = 0;
        virtual void on_chunk(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) = 0;
        virtual void on_end_group(chunk_type type) = 0;
    };

    /**
     * Utility chunks factory. Every chunk type should be of form
     * struct vport
     *   {
     *   static constexpr auto CHUNK_ID = formats::iff::fourcc("CAMG");
     *   static vport read(std::istream& is, std::size_t size);
     *  };
     */

    namespace detail
    {

        template<int I, typename ... Chunks>
        std::variant<std::monostate, Chunks...> parse_chunk_helper ([[maybe_unused]] formats::iff::chunk_type type,
                                                                    [[maybe_unused]] std::istream& is,
                                                                    [[maybe_unused]] std::size_t size)
        {
            if constexpr (I == sizeof...(Chunks))
            {
                return std::monostate{};
            }
            else
            {
                using chunk_t = typename std::tuple_element<I, std::tuple<Chunks...>>::type;
                if (chunk_t::CHUNK_ID == type)
                {
                    return chunk_t::read(is, size);
                }
                else
                {
                    return parse_chunk_helper<I+1, Chunks...>(type, is, size);
                }
            }
        }
    }

    template<typename ... Chunks>
    std::variant<std::monostate, Chunks...> parse_chunk (formats::iff::chunk_type type,
                                                         std::istream& is,
                                                         std::size_t size)
    {
        return detail::parse_chunk_helper<0, Chunks...>(type, is, size);
    }
}

#endif //TOMBEXCAVATOR_IFF_EVENTS_HH
