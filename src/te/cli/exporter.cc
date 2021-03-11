//
// Created by igor on 10/03/2021.
//

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
    exporter::tile_sheet ts(sg);
    auto opng = m_odir / (m_name + ".png");
    auto otsx = m_odir / (m_name + ".tsx");
    exporter::to_tsx(ts, m_name, otsx, opng);
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