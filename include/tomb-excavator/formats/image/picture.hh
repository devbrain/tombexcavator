//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_HH
#define TOMBEXCAVATOR_PICTURE_HH

#include <tomb-excavator/export-formats.h>
#include <vector>
#include <filesystem>
#include <iosfwd>
#include <variant>
#include <optional>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace formats::image
{
    struct FORMATS_API rgb
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct FORMATS_API rgba
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    using rgb_palette = std::vector<rgb>;
    using rgba_palette = std::vector<rgba>;

    class FORMATS_API indexed_color_mapper
    {
    public:
        explicit indexed_color_mapper(const rgb_palette& pal);
        explicit indexed_color_mapper(const rgba_palette& pal);
        indexed_color_mapper(const rgb_palette& pal, uint8_t key_color);
        indexed_color_mapper(const rgba_palette& pal, uint8_t key_color);

        rgba operator[] (uint8_t color) const;
    private:
        std::variant<rgb_palette, rgba_palette> m_pal;
        std::optional<uint8_t> m_key_color;
    };

    struct FORMATS_API picture
    {
        unsigned width;
        unsigned height;
        int bpp; // 3 for RGB and 4 for RGBA

        std::vector<char> pixels;

        picture() = default;
        picture(unsigned w, unsigned h, bool is_rgba);

        void put(unsigned x, unsigned y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        void put(unsigned x, unsigned y, uint8_t color, const indexed_color_mapper& mapper);
        void put(unsigned x, unsigned y, const rgb& color);
        void put(unsigned x, unsigned y, const rgba& color);
    };

    FORMATS_API bool save_to_png(const picture& pic, const std::filesystem::path& path);
    FORMATS_API bool save_to_png(const picture& pic, std::vector<char>& out);
    FORMATS_API picture load_picture(const std::filesystem::path& path);
    FORMATS_API picture load_picture(const char* data, std::size_t size);
    FORMATS_API picture load_picture(std::istream& is);
} // ns

#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_PICTURE_HH
