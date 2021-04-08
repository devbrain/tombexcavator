//
// Created by igor on 04/04/2021.
//

#include <istream>
#include <algorithm>
#include <tomb-excavator/formats/iff/ea/ea_events.hh>
#include <tomb-excavator/bsw/byte_order.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace formats::iff
{
    bool ea_events::on_start_group(std::istream& is, chunk_type type, [[maybe_unused]] uint64_t offset,
                                   [[maybe_unused]] std::size_t size)
    {
        if (type == fourcc("FORM"))
        {
            auto tag = read_chunk_type(is);
            m_forms.push(tag);
            on_form_start(tag);
        } else
        {
            if (type == fourcc("CAT "))
            {
                on_cat_start();
            } else
            {
                if (type == fourcc("LIST"))
                {
                    on_list_start();
                } else
                {
                    RAISE_EX("Unsupported group type ", type);
                }
            }
        }
        return true;
    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_end_group(chunk_type type)
    {
        if (type == fourcc("FORM"))
        {
            if (m_forms.empty())
            {
                RAISE_EX("Corrupted IFF");
            }
            auto tag = m_forms.top();
            m_forms.pop();
            on_form_end(tag);
        } else
        {
            if (type == fourcc("CAT "))
            {
                on_cat_end();
            } else
            {
                if (type == fourcc("LIST"))
                {
                    on_list_end();
                }
            }
        }
    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_form_start([[maybe_unused]] chunk_type name)
    {

    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_form_end([[maybe_unused]] chunk_type name)
    {

    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_list_start()
    {

    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_list_end()
    {

    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_cat_start()
    {

    }
    // -------------------------------------------------------------------------------------------------
    void ea_events::on_cat_end()
    {

    }
    // -------------------------------------------------------------------------------------------------
    chunk_type ea_events::read_chunk_type(std::istream& is)
    {
        uint32_t dword;
        is.read((char*) &dword, sizeof(dword));
        return chunk_type{bsw::byte_order::from_big_endian(dword)};
    }
    // ================================================================================================
    ea_tester::ea_tester(std::initializer_list<chunk_type> expected)
            : m_valid(false), m_expected(expected)
    {}
    // -------------------------------------------------------------------------------------------------
    bool ea_tester::valid() const
    {
        return m_valid;
    }
    // -------------------------------------------------------------------------------------------------
    bool ea_tester::on_start_group(std::istream& is, chunk_type type,
                                   [[maybe_unused]] uint64_t offset, [[maybe_unused]] std::size_t size)
    {
        if (type == fourcc("FORM"))
        {
            auto tag = read_chunk_type(is);
            m_valid = (std::find(m_expected.begin(), m_expected.end(), tag) != m_expected.end());
        }
        return false;
    }
    // -------------------------------------------------------------------------------------------------
    void ea_tester::on_end_group([[maybe_unused]] chunk_type type)
    {

    }
    // -------------------------------------------------------------------------------------------------
    void ea_tester::on_chunk([[maybe_unused]] std::istream& is, [[maybe_unused]] chunk_type type,
                             [[maybe_unused]] uint64_t offset, [[maybe_unused]] std::size_t size)
    {

    }
}
