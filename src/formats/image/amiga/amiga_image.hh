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
#include <tomb-excavator/formats/iff/fourcc.hh>


namespace formats::image::amiga
{
    using LONG = uint32_t;
    using WORD = uint16_t;
    using BYTE = uint8_t;


    struct bmhd_entry
    {
        WORD Width;        /* Width of image in pixels */
        WORD Height;       /* Height of image in pixels */
        WORD Left;         /* X coordinate of image */
        WORD Top;          /* Y coordinate of image */
        BYTE Bitplanes;    /* Number of bitplanes */
        BYTE Masking;      /* Type of masking used */
        BYTE Compress;     /* Compression method use on image data */
        BYTE Padding;      /* Alignment padding (always 0) */
        WORD Transparency; /* Transparent background color */
        BYTE XAspectRatio; /* Horizontal pixel size */
        BYTE YAspectRatio; /* Vertical pixel size */
        WORD PageWidth;    /* Horizontal resolution of display device */
        WORD PageHeight;   /* Vertical resolution of display device */
    };

    struct cmap_entry
    {
        BYTE Red;           /* Red color component (0-255) */
        BYTE Green;         /* Green color component (0-255) */
        BYTE Blue;          /* Blue color component (0-255) */
    };

    struct cmyk_entry
    {
        BYTE cyan, magenta, yellow, black;
    };

    struct color_names
    {
        WORD starting_color;
        WORD ending_color;
        std::vector<std::string> names;
    };

    struct color_range
    {
        WORD pad1;
        WORD rate;
        WORD active;
        BYTE low, high;
    };

    struct dest_merge
    {
        BYTE depth;
        BYTE pad1;
        WORD planePick;
        WORD planeOnOff;
        WORD planeMask;
    };

    struct dpi_header
    {
        WORD dpiX, dpiY;
    };

#define ILBM_DRANGE_60_STEPS_PER_SECOND 16384

#define ILBM_RNG_ACTIVE 1
#define ILBM_RNG_DP_RESERVED 4
#define ILBM_RNG_FADE 8

    struct dcolor
    {
        BYTE cell;
        BYTE r, g, b;
    };

    struct dindex
    {
        BYTE cell;
        BYTE index;
    };

    struct dfade
    {
        BYTE cell;
        BYTE fade;
    };

    struct drange
    {
        BYTE min;
        BYTE max;
        WORD rate;
        WORD flags;
        BYTE ntrue;
        BYTE nregs;

        std::vector<dcolor> dcolors;
        std::vector<dindex> dindexes;

        BYTE nfades;
        BYTE pad;
        std::vector<dfade> dfades;
    };

    struct point2d
    {
        WORD x, y;
    };

    struct sprite
    {
        WORD precedence;
    };

    struct vport
    {
        uint32_t mode;
    };

    struct cycle_info
    {
        WORD direction;
        BYTE start, end;
        LONG seconds;
        LONG microSeconds;
        WORD pad;
    };

    enum class image_type
    {
        ILBM,
        PBM,
        ACBM
    };

    struct image
    {
        image_type type;
        bmhd_entry bmhd;
        std::vector<cmap_entry> cmap;
        std::vector<cmyk_entry> cmyk;
        std::optional<color_names> cnames;
        std::vector<color_range> crng;
        std::optional<dest_merge> dm;
        std::optional<dpi_header> dpi;
        std::vector<drange> drng;
        std::optional<point2d> grab;
        std::optional<sprite> sprt;
        std::optional<vport> view_port;
        std::vector<cycle_info> cycles;
        std::vector<unsigned char> body;
        std::vector<unsigned char> bitplanes;

        void update(std::istream& is, formats::iff::chunk_type type, std::size_t size);
    };
    /*

    */
}

#endif //TOMBEXCAVATOR_AMIGA_IMAGE_HH
