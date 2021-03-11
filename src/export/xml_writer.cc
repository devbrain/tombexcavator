//
// Created by igor on 04/03/2021.
//

#include <tomb-excavator/export/xml_writer.hh>

namespace exporter
{
    xml_stream::xml_stream(std::ostream& os, bool pretty)
            : m_ostream(os),
            m_pretty (pretty)
    {

    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::ident()
    {
        if (m_pretty)
        {
            const auto nesting = m_stack.size();
            for (std::size_t i = 0; i < 4 * nesting; i++)
            {
                m_ostream << ' ';
            }
        }
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::eol()
    {
        if (m_pretty)
        {
            m_ostream << '\n';
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
                m_ostream << ' ';
            }
            m_ostream << a.first << R"(=")" << a.second.val << R"(")";
            first = false;
        }
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::start_element(const std::string& name, const std::initializer_list<attrib_t>& attribs)
    {
        ident();
        if (!is_parent_closed())
        {
            m_ostream << ">";
            eol();
            set_parent_close();
        }
        m_ostream << '<' << name;
        if (attribs.size() > 0)
        {
            m_ostream << ' ';
        }
        write_attribs(attribs);
        add_child_to_parent();
        m_stack.push({name, false, false});
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::end_element()
    {
        if (m_stack.empty())
        {
            return;
        }
        if (is_parent_has_children())
        {
            auto name = m_stack.top().name;
            m_stack.pop();

            ident();
            m_ostream << "</" << name << ">";
            eol();
        }
        else
        {
            m_stack.pop();
            m_ostream << " />";
            eol();
        }

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
        m_ostream << "?>\n";
    }
    // ----------------------------------------------------------------------------------------
    xml_stream::operator bool() const noexcept
    {
        return true;
    }
    // ----------------------------------------------------------------------------------------
    bool xml_stream::is_parent_closed() const
    {
        if (m_stack.empty())
        {
            return true;
        }
        return m_stack.top().is_closed;
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::set_parent_close()
    {
        if (!m_stack.empty())
        {
            m_stack.top().is_closed = true;
        }
    }
    // ----------------------------------------------------------------------------------------
    void xml_stream::add_child_to_parent()
    {
        if (!m_stack.empty())
        {
            m_stack.top().has_children = true;
        }
    }
    // ----------------------------------------------------------------------------------------
    bool xml_stream::is_parent_has_children() const
    {
        if (m_stack.empty())
        {
            return false;
        }
        return m_stack.top().has_children;
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
