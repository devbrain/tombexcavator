//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_EA_EVENTS_HH
#define TOMBEXCAVATOR_EA_EVENTS_HH

#include <tomb-excavator/formats/iff/iff_events.hh>
#include <vector>
#include <stack>

namespace formats::iff
{
    class FORMATS_API ea_events : public iff_events
    {
    protected:
        static chunk_type read_chunk_type(std::istream& is) ;
    protected:
        virtual void on_form_start(chunk_type name);
        virtual void on_form_end(chunk_type name);

        virtual void on_list_start();
        virtual void on_list_end();

        virtual void on_cat_start();
        virtual void on_cat_end();
    private:
        bool on_start_group(std::istream& is, chunk_type type, uint64_t offset, std::size_t size) override;
        void on_end_group(chunk_type type) override;
    private:
        std::stack<chunk_type> m_forms;
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
