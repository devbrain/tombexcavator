//
// Created by igor on 04/04/2021.
//

#include <tomb-excavator/formats/iff/iff_events.hh>

namespace formats::iff
{
    iff_events::~iff_events() = default;

    void iff_events::read_header([[maybe_unused]] std::istream& is, [[maybe_unused]] uint64_t file_size)
    {

    }
}