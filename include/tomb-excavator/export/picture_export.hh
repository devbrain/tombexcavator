//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_EXPORT_HH
#define TOMBEXCAVATOR_PICTURE_EXPORT_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/export/transformer.hh>
#include <tomb-excavator/provider/dto/video/picture.hh>
#include <filesystem>

namespace exporter
{
class EXPORT_API picture_export : public transformer<provider::dto::picture>
    {
    public:
        explicit picture_export(std::filesystem::path opath);
    private:
        void transform(const provider::dto::picture& pic) override;
    private:
        std::filesystem::path m_opath;
    };
}

#endif //TOMBEXCAVATOR_PICTURE_EXPORT_HH
