//
// Created by igor on 04/03/2021.
//

#include <tomb-excavator/export/xml_writer.hh>

namespace exporter
{
    xml_stream::xml_stream(std::ostream& os)
            : m_ostream(os)
    {

    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::ident()
    {
        const auto nesting = m_stack.size();
        for (std::size_t i = 0; i<4*nesting; i++)
        {
            m_ostream << ' ';
        }
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::write_attribs(const attrib_list_t& attribs)
    {
        bool first = true;
        for (const auto& a : attribs)
        {
            if (!first)
            {
                m_ostream << " ";
            }
            m_ostream << a.first << R"(=")" << a.second << R"(")";
            first = false;
        }
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::start_element(const std::string& name, const std::initializer_list<attrib_t>& attribs)
    {
        ident();
        m_ostream << "<" << name;
        if (attribs.size() > 0)
        {
            m_ostream << ' ';
        }
        write_attribs(attribs);
        m_ostream << ">\n";
        m_stack.push(name);
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::end_element()
    {
        if (m_stack.empty())
        {
            return;
        }
        auto name = m_stack.top();
        m_stack.pop();

        ident();
        m_ostream << "</" << name << ">\n";

    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::prolog(const attrib_list_t& attribs)
    {
        m_ostream << "<?xml ";
        if (attribs.size() == 0)
        {
            m_ostream << R"(version="1.0" encoding="UTF-8")";
        }
        else
        {
            write_attribs(attribs);
        }
        m_ostream << "?>";
    }
    // ----------------------------------------------------------------------------------------
    xml_stream::operator bool() const noexcept
    {
        return true;
    }
    // =========================================================================================
    xml_doc_writer::xml_doc_writer(xml_stream& stream, const std::string& node, const xml_stream::attrib_list_t& atts)
    : m_stream(stream)
            {
                    m_stream.start_element(node, atts);
            }
    // ----------------------------------------------------------------------------------------
    xml_doc_writer::~xml_doc_writer()
    {
        m_stream.end_element();
    }
    // ----------------------------------------------------------------------------------------
    xml_doc_writer::operator bool() const noexcept
    {
        return true;
    }
}
