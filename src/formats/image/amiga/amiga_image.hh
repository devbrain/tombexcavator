//
// Created by igor on 05/04/2021.
//

#ifndef TOMBEXCAVATOR_AMIGA_IMAGE_HH
#define TOMBEXCAVATOR_AMIGA_IMAGE_HH

#include <cstdint>
#include <vector>
#include <string>
#include <iosfwd>
#include <optional>

#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/formats/iff/fourcc.hh>

namespace formats::image::amiga
{
    enum class image_type
    {
        ILBM,
        PBM,
        ACBM
    };

    enum class masking_type
    {
            NONE,
            HAS_MASK,
            HAS_TRANSPARENT_COLOR,
            LASSO
    };

    enum class compression_type
    {
        NONE,
        BYTE_RUN
    };

    struct image
    {
        explicit image(image_type t);
        image_type type;

        uint16_t w, h;
        uint8_t  nPlanes;
        masking_type masking;
        compression_type compression;
        uint16_t transparentColor;
        uint16_t pageWidth, pageHeight;

        rgb_palette colors;
        std::optional<uint32_t> viewport_mode;
        std::vector<uint8_t> body;

        void update(formats::iff::chunk_type chunk_type, std::istream& is, std::size_t size);
        void convert(formats::image::picture& out);
    };

}

#endif //TOMBEXCAVATOR_AMIGA_IMAGE_HH
