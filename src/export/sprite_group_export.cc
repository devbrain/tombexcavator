//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/export/sprite_group_export.hh>
#include <tomb-excavator/export/xml_writer.hh>
#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <fstream>
#include <algorithm>
#include <sstream>


namespace exporter
{
    void to_png(const provider::dto::tile_sheet& ts, const std::filesystem::path& oname)
    {
        auto file = std::fstream(oname, std::ios::out | std::ios::binary);

        if (!file)
        {
            RAISE_EX("Failed to create file ", oname);
        }
        file.write(ts.png().data(), ts.png().size());
        file.close();
    }
    // -------------------------------------------------------------------------------------------
    void to_tsx(const provider::dto::tile_sheet& ts,
                const std::string& tileset_name,
                const std::filesystem::path& oname_tmx,
                const std::filesystem::path& oname_png)
    {
        to_png(ts, oname_png);
        std::ostringstream os;
        START_XML(os)
        {
            XML_PROLOG();
            XML_NODE("tileset", {"name", tileset_name},
                     {"tilewidth", ts.sprite_dim().w()}, {"tileheight", ts.sprite_dim().h()}, {"spacing", 0}, {"margins", 0}, {"columns", 1},
                     {"tilecount", ts.size()})
            {
                XML_NODE("image", {"source", oname_tmx}, {"width", ts.dim().w()}, {"height", ts.dim().h()});
            }
        }
        std::ofstream ofs(oname_tmx, std::ios::binary | std::ios::out);
        const auto s = os.str();
        ofs.write(s.c_str(), s.size());
    }
}
