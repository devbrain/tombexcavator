//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_IFF_EVENTS_HH
#define TOMBEXCAVATOR_IFF_EVENTS_HH

#include <tomb-excavator/export-formats.h>
#include <tomb-excavator/formats/iff/fourcc.hh>
#include <iosfwd>

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
}

#endif //TOMBEXCAVATOR_IFF_EVENTS_HH
