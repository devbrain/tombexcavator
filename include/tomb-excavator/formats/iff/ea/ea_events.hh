//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_EA_EVENTS_HH
#define TOMBEXCAVATOR_EA_EVENTS_HH

#include <tomb-excavator/formats/iff/iff_events.hh>
#include <vector>

namespace formats::iff
{
    class FORMATS_API ea_events : public iff_events
    {
    protected:
        chunk_type read_chunk_type(std::istream& is) const;
    private:
        bool on_start_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) override;
        void on_end_group(chunk_type type) override;
    };

    class FORMATS_API ea_tester : public ea_events
    {
    public:
        ea_tester(std::initializer_list<chunk_type> expected);
        [[nodiscard]] bool valid() const;
    private:
        bool on_start_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) override;
        void on_chunk(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) override;
    private:
        bool m_valid;
        std::vector<chunk_type> m_expected;
    };
}

#endif //TOMBEXCAVATOR_EA_EVENTS_HH
