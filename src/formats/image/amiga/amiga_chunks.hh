//
// Created by igor on 07/04/2021.
//

#ifndef TOMBEXCAVATOR_AMIGA_CHUNKS_HH
#define TOMBEXCAVATOR_AMIGA_CHUNKS_HH

#include <cstdint>
#include <iosfwd>
#include <vector>
#include <variant>
#include <tuple>
#include <tomb-excavator/formats/iff/fourcc.hh>


namespace formats::amiga
{
    using LONG = uint32_t;
    using WORD = uint16_t;
    using BYTE = uint8_t;


    struct bmhd
    {
        static constexpr auto CHUNK_ID = formats::iff::fourcc("BMHD");
        static bmhd read(std::istream& is, std::size_t size);

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

    struct cmap
    {
        static constexpr auto CHUNK_ID = formats::iff::fourcc("CMAP");
        static cmap read(std::istream& is, std::size_t size);

        struct cmap_entry
        {
            BYTE Red;           /* Red color component (0-255) */
            BYTE Green;         /* Green color component (0-255) */
            BYTE Blue;          /* Blue color component (0-255) */
        };

        std::vector<cmap_entry> colors;
    };

    struct vport
    {
        static constexpr auto CHUNK_ID = formats::iff::fourcc("CAMG");
        static vport read(std::istream& is, std::size_t size);
        LONG mode;
    };

    struct body
    {
        static constexpr auto CHUNK_ID = formats::iff::fourcc("BODY");
        static body read(std::istream& is, std::size_t size);

        std::vector<uint8_t> data;
    };

    struct bitplanes
    {
        static constexpr auto CHUNK_ID = formats::iff::fourcc("ABIT");
        static bitplanes read(std::istream& is, std::size_t size);

        std::vector<uint8_t> data;
    };


}

#endif //TOMBEXCAVATOR_AMIGA_CHUNKS_HH