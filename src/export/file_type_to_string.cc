//
// Created by igor on 08/03/2021.
//


#include <tomb-excavator/export/file_type_to_string.hh>

namespace
{
    struct visitor
    {
        std::string operator () (const provider::dto::sprite_group& )
        {
            return "tilesheet";
        }
        std::string operator () (const provider::dto::picture&)
        {
            return "image";
        }
    };
}

namespace exporter
{
    std::string file_type_to_string(const provider::file_type_t& ft)
    {
        return std::visit(visitor{}, ft);
    }
}
