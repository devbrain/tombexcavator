//
// Created by igor on 05/04/2021.
//

#include <vector>
#include <cstring>
#include "formats/image/amiga/amiga_image.hh"
#include "amiga_utils.hh"
extern "C"
{
#include "formats/image/thirdparty/libamivideo/amivideotypes.h"
#include "formats/image/thirdparty/libamivideo/palette.h"
#include "formats/image/thirdparty/libamivideo/screen.h"
#include "formats/image/thirdparty/libamivideo/viewportmode.h"
}
#include <tomb-excavator/bsw/exceptions.hh>

namespace formats::image::amiga
{


    enum ILBM_Compression
    {
        ILBM_CMP_BYTE_RUN = 1
    };

    // ---------------------------------------------------------------------------------------------------------------
    unsigned int calculate_row_size(const image* img)
    {
        unsigned int rowSizeInWords = img->bmhd.Width / 16;

        if (img->bmhd.Width % 16 != 0)
        {
            rowSizeInWords++;
        }

        return (rowSizeInWords * 2);
    }
    // ---------------------------------------------------------------------------------------------------------------
    std::vector<uint8_t> unpack_byte_run(const image* img)
    {
        std::vector<uint8_t> decompressedChunkData;
        /* Only perform decompression if the body is compressed and present */
        if (img->bmhd.Compress == ILBM_CMP_BYTE_RUN && !img->body.empty())
        {
            /* Counters */
            unsigned int count = 0;
            unsigned int readBytes = 0;

            /* Allocate decompressed chunk attributes */

            std::size_t chunkSize = calculate_row_size(img) * img->bmhd.Height * img->bmhd.Bitplanes;
            decompressedChunkData.resize(chunkSize);


            /* Perform RLE decompression */

            while (readBytes < img->body.size())
            {
                int byte = img->body[readBytes];
                readBytes++;

                if (byte >= 0 && byte <= 127) /* Take the next byte bytes + 1 literally */
                {
                    int i;

                    for (i = 0; i < byte + 1; i++)
                    {
                        decompressedChunkData[count] = img->body[readBytes];
                        readBytes++;
                        count++;
                    }
                } else
                {
                    if (byte >= -127 && byte <= -1) /* Replicate the next byte, -byte + 1 times */
                    {
                        uint8_t ubyte;
                        int i;

                        ubyte = img->body[readBytes];
                        readBytes++;

                        for (i = 0; i < -byte + 1; i++)
                        {
                            decompressedChunkData[count] = ubyte;
                            count++;
                        }
                    } else
                    {
                        RAISE_EX("Unknown byte run encoding byte!");
                    }
                }
            }
        }
        return decompressedChunkData;
    }
    // ---------------------------------------------------------------------------------------------------
    unsigned int calculate_num_of_colors(const bmhd_entry* bitMapHeader)
    {
        switch (bitMapHeader->Bitplanes)
        {
            case 1:
                return 2;
            case 2:
                return 4;
            case 3:
                return 8;
            case 4:
                return 16;
            case 5:
                return 32;
            case 6:
                return 64;
            case 7:
                return 128;
            case 8:
                return 256;
            default:
                return 0;
        }
    }
    // -----------------------------------------------------------------------------------------------
    std::vector<amiVideo_Color> generate_grayscale_color_map(const image* img)
    {

        std::vector<amiVideo_Color> colorMap;

        unsigned int numOfColors = calculate_num_of_colors(&img->bmhd);
        unsigned int i;

        for (i = 0; i < numOfColors; i++)
        {
            unsigned int value = i * 0xff / (numOfColors - 1);
            amiVideo_Color colorRegister;
            colorRegister.r = value;
            colorRegister.g = value;
            colorRegister.b = value;
            colorMap.push_back(colorRegister);
        }

        return colorMap;
    }

    // -----------------------------------------------------------------------------------------------
    void init_palette_from_image(const image* image, amiVideo_Palette* palette)
    {
        if (image->cmap.empty())
        {
            /* If no colormap is provided by the image, use a generated grayscale one */
            auto colorMap = generate_grayscale_color_map(image);
            amiVideo_setBitplanePaletteColors(palette, colorMap.data(),
                                              colorMap.size());
        } else
        {
            amiVideo_setBitplanePaletteColors(palette, (amiVideo_Color*) image->cmap.data(),
                                              image->cmap.size());
        } /* Otherwise, use the provided color map */
    }
    // -----------------------------------------------------------------------------------------------
    amiVideo_ULong extract_viewport_mode_from_image(const image* image)
    {
        amiVideo_ULong paletteFlags, resolutionFlags;

        if (!image->view_port)
        {
            paletteFlags = 0; /* If no viewport value is set, assume 0 value */
        } else
        {
            paletteFlags = amiVideo_extractPaletteFlags(image->view_port->mode);
        } /* Only the palette flags can be considered "reliable" from a viewport mode value */

        /* Resolution flags are determined by looking at the page dimensions */
        resolutionFlags = amiVideo_autoSelectViewportMode(image->bmhd.PageWidth,
                                                          image->bmhd.PageHeight);

        /* Return the combined settings of the previous */
        return paletteFlags | resolutionFlags;
    }
    // -----------------------------------------------------------------------------------------------
#define MAX_NUM_OF_BITPLANES 32

    void
    deinterleave_to_bitplane_memory(const image* img, const std::vector<uint8_t>& body, uint8_t** bitplanePointers)
    {
        if (!body.empty())
        {
            unsigned int i;
            int count = 0; /* Offset in the interleaved source */
            int hOffset = 0; /* Horizontal offset in resulting bitplanes */
            unsigned int rowSize = calculate_row_size(img);

            for (i = 0; i < img->bmhd.Height; i++)
            {
                unsigned int j;

                for (j = 0; j < img->bmhd.Bitplanes; j++)
                {
                    memcpy(bitplanePointers[j] + hOffset, body.data() + count, rowSize);
                    count += rowSize;
                }

                hOffset += rowSize;
            }
        }
    }
    // ---------------------------------------------------------------------------------------------------------------
    std::vector<uint8_t> deinterleave(const image* img, const std::vector<uint8_t>& body)
    {
        int nPlanes = img->bmhd.Bitplanes;
        unsigned int bitplaneSize = calculate_row_size(img) * img->bmhd.Height;
        std::vector<uint8_t> result(bitplaneSize * nPlanes);

        unsigned int i;
        unsigned int offset = 0;
        uint8_t* bitplanePointers[MAX_NUM_OF_BITPLANES];

        /* Set bitplane pointers */

        for (i = 0; i < nPlanes; i++)
        {
            bitplanePointers[i] = result.data() + offset;
            offset += bitplaneSize;
        }

        /* Deinterleave and write results to the bitplane addresses */
        deinterleave_to_bitplane_memory(img, body, bitplanePointers);

        /* Return result */
        return result;

    }
    // -----------------------------------------------------------------------------------------------
    void attach_image_to_screen(const image* img, amiVideo_Screen* screen)
    {
        /* Determine which viewport mode is best for displaying the image */
        auto viewportMode = extract_viewport_mode_from_image(img);

        /* Initialize the screen with the image's dimensions, bitplane depth, and viewport mode */
        amiVideo_initScreen(screen, img->bmhd.Width, img->bmhd.Height, img->bmhd.Bitplanes, 8,
                            viewportMode);

        /* Sets the colors of the palette */
        init_palette_from_image(img, &screen->palette);

        /* Decompress the image body */
        auto decompressed = unpack_byte_run(img);
        const std::vector<uint8_t>* body = decompressed.empty() ? &img->body : &decompressed;

        /* Attach the appropriate pixel surface to the screen */
        if (img->type == image_type::PBM)
        {
            amiVideo_setScreenUncorrectedChunkyPixelsPointer(screen, const_cast<uint8_t*>(body->data()),
                                                             img->bmhd.Width); /* A PBM has chunky pixels in its body */
        } else
        {
            if (img->type == image_type::ACBM)
            {
                amiVideo_setScreenBitplanes(screen,
                                            (amiVideo_UByte*) img->bitplanes.data()); /* Set bitplane pointers of the conversion screen */
            } else
            {
                if (img->type == image_type::ILBM)
                {
                    auto bitplanes = deinterleave(img, *body);
                    /* Amiga ILBM image has interleaved scanlines per bitplane. We have to deinterleave it in order to be able to convert it */
                    amiVideo_setScreenBitplanes(screen,
                                                (amiVideo_UByte*) bitplanes.data());
                    /* Set bitplane pointers of the conversion screen */
                }
            }
        }
    }
    // -----------------------------------------------------------------------------------------------------------

}