//
// Created by igor on 10/03/2021.
//

#include <fstream>
#include "exporter.hh"
#include <tomb-excavator/export/sprite_group_export.hh>
#include <tomb-excavator/export/picture_export.hh>
#include <tomb-excavator/formats/image/picture.hh>

data_exporter::data_exporter(const std::string& name, const std::filesystem::path& odir)
: m_name(name),
  m_odir(odir)
{

}
// ------------------------------------------------------------------------------------------------
void data_exporter::operator () (const provider::dto::sprite_group& sg) const
{
    const auto sheets = provider::dto::tile_sheet::create(sg);
    int i = 0;
    for (const auto& ts : sheets)
    {
        std::string base = m_name;
        if (sheets.size() > 1)
        {
            std::ostringstream os;
            os << m_name + "-" << i;
            base = os.str();
            i++;
        }

        auto opng = m_odir / (base + ".png");
        auto otsx = m_odir / (base + ".tsx");
        exporter::to_tsx(ts, base, otsx, opng);
    }
}
// ------------------------------------------------------------------------------------------------
void data_exporter::operator () (const provider::dto::picture& pic) const
{
    auto opath = m_odir / (m_name + ".png");
    exporter::to_png(pic, opath);
}
// ------------------------------------------------------------------------------------------------
void data_exporter::operator () (const formats::image::picture& pic) const
{
    auto opath = m_odir / (m_name + ".png");
    formats::image::save_to_png(pic, opath);
}
// ------------------------------------------------------------------------------------------------
void data_exporter::operator () (const std::string& txt) const
{
    auto opath = m_odir / (m_name + ".txt");
    std::ofstream ofs (opath, std::ios::binary | std::ios::out);
    ofs.write(txt.c_str(), txt.size());
}