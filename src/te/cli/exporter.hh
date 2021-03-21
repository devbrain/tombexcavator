//
// Created by igor on 10/03/2021.
//

#ifndef TOMBEXCAVATOR_EXPORTER_HH
#define TOMBEXCAVATOR_EXPORTER_HH

#include <string>
#include <filesystem>

#include <tomb-excavator/provider/file_types.hh>

class data_exporter
{
public:
    data_exporter(const std::string& name, const std::filesystem::path& odir);

    void operator () (const provider::dto::sprite_group& sg) const;
    void operator () (const provider::dto::map& map) const;
    void operator () (const provider::dto::picture& pic) const;
    void operator () (const formats::image::picture& pic) const;
    void operator () (const std::string& txt) const;

private:
    std::string m_name;
    std::filesystem::path m_odir;
};


#endif //TOMBEXCAVATOR_EXPORTER_HH
