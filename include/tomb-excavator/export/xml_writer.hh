//
// Created by igor on 04/03/2021.
//

#ifndef TOMBEXCAVATOR_XML_WRITER_HH
#define TOMBEXCAVATOR_XML_WRITER_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/bsw/macros.hh>
#include <iostream>
#include <utility>
#include <string>
#include <stack>
#include <sstream>
#include <tomb-excavator/msvc/c4251-begin.h>
namespace exporter
{
    struct EXPORT_API xml_string_rep
    {
        template <typename T>
        xml_string_rep(const T& x)
        {
            std::ostringstream os;
            os << x;
            val = os.str();
        }
        std::string val;
    };

    class EXPORT_API xml_stream
    {
    public:
        using attrib_t = std::pair<std::string, xml_string_rep>;
        using attrib_list_t =  std::initializer_list<attrib_t>;
    public:
        explicit xml_stream(std::ostream& os, bool pretty = true);

        void start_element(const std::string& name, const attrib_list_t& attribs);
        void end_element();

        explicit operator bool() const noexcept;

        template<typename ... Args>
        void text(Args&&... args)
        {
            if constexpr (sizeof...(args) > 0)
            {
                ((m_ostream << std::forward<Args>(args)), ...);
                m_ostream << "\n";
            }
        }

        template<typename ... Args>
        void comment(Args&&... args)
        {
            m_ostream << "\n<!-- ";
            if constexpr (sizeof...(args) > 0)
            {
                ((m_ostream << std::forward<Args>(args)), ...);
                m_ostream << "\n";
            }
            m_ostream << " --> \n";
        }

        void prolog(const attrib_list_t& attribs);
    private:
        void write_attribs(const attrib_list_t& attribs);
        void ident();
        void eol();

        [[nodiscard]] bool is_parent_closed() const;
        void set_parent_close();
        void add_child_to_parent();
        [[nodiscard]] bool is_parent_has_children() const;
    private:
        struct node_info
        {
            std::string name;
            bool has_children;
            bool is_closed;
        };
    private:
        std::ostream& m_ostream;
        bool m_pretty;
        std::stack<node_info> m_stack;
    };
    // ===================================================================================
    class EXPORT_API xml_doc_writer
    {
    public:
        xml_doc_writer(xml_stream& stream, const std::string& node, const xml_stream::attrib_list_t& atts);
        ~xml_doc_writer();
        explicit operator bool() const noexcept;
    private:
        xml_stream& m_stream;
    };

}

#include <tomb-excavator/msvc/c4251-end.h>

#define CURRENT_XML_STREAM get_xml_stream

#define START_XML(OSTREAM, ...) \
    if (auto get_xml_stream = [xstream = ::exporter::xml_stream(OSTREAM, ##__VA_ARGS__ )]() mutable -> ::exporter::xml_stream& { return const_cast<::exporter::xml_stream&>(xstream); }; get_xml_stream())

#define CONTINUE_XML(XML_STREAM) \
    if (auto get_xml_stream = [&XML_STREAM]() -> ::exporter::xml_stream& { return const_cast<::exporter::xml_stream&>(XML_STREAM); }; get_xml_stream())

#define XML_NODE(NAME, ...) if (::exporter::xml_doc_writer ANONYMOUS_VAR(xnw)(get_xml_stream(), NAME, { __VA_ARGS__ }); ANONYMOUS_VAR(xnw))
#define XML_TEXT(...) get_xml_stream().text(__VA_ARGS__)
#define XML_COMMENT(...) get_xml_stream().comment(__VA_ARGS__)
#define XML_PROLOG(...) get_xml_stream().prolog({ __VA_ARGS__ })

#if defined(__GNUC__) || defined(__GNUG__)
#pragma GCC diagnostic ignored "-Wempty-body"
#elif defined(_MSC_VER)
#pragma warning( disable : 4390 )

#endif


#endif //TOMBEXCAVATOR_XML_WRITER_HH
