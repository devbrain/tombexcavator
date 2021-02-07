#ifndef ARCHIVE_OUTPUT_HH
#define ARCHIVE_OUTPUT_HH

#include <archive/archive_api.h>
#include "palette.hh"
#include <string>
#include <vector>
#include <tuple>


namespace archive
{
    ARCHIVE_API bool save_palette(const std::string& fname, const palette& pal);

    ARCHIVE_API bool save_pixels(const std::string& fname,
            const unsigned char* pixels, std::size_t w, std::size_t h,
            const palette& pal);
}

#endif