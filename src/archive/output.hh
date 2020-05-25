#ifndef ARCHIVE_OUTPUT_HH
#define ARCHIVE_OUTPUT_HH

#include <archive/archive_api.h>
#include <string>
#include <vector>
#include <tuple>
namespace archive
{
    ARCHIVE_API bool save_palette(const std::string& fname, const unsigned char* pal, size_t num_colors);
    ARCHIVE_API bool save_palette(const std::string& fname, const std::vector<unsigned char>& pal);

    ARCHIVE_API bool save_pixels(const std::string& fname,
            const unsigned char* pixels, std::size_t w, std::size_t h,
            const unsigned char* pal, size_t num_colors);
    ARCHIVE_API bool save_pixels(const std::string& fname,
                                 const std::vector<unsigned char>& pixels, std::size_t w, std::size_t h,
                                 const std::vector<unsigned char>& pal);
    struct ARCHIVE_API pixels_frame
    {
        pixels_frame(std::size_t width, std::size_t height);

        std::vector<unsigned char> pixels;
        std::size_t w;
        std::size_t h;
    };

    ARCHIVE_API bool save_pixels(const std::string& fname,
                                 const pixels_frame& frame,
                                 const std::vector<unsigned char>& pal);

    ARCHIVE_API bool save_pixels(const std::string& fname,
                                 const std::vector<pixels_frame>& frames,
                                 const std::vector<unsigned char>& pal);
}

#endif