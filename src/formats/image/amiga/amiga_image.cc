//
// Created by igor on 05/04/2021.
//

#include "amiga_image.hh"
#include <tomb-excavator/bsw/exceptions.hh>
#include <tomb-excavator/bsw/override.hh>
#include <tomb-excavator/tombexcavator_configure.h>
#include "formats/image/amiga/amiga_chunks.hh"


extern "C"
{
#include "formats/image/thirdparty/libamivideo/palette.h"
#include "formats/image/thirdparty/libamivideo/screen.h"
#include "formats/image/thirdparty/libamivideo/viewportmode.h"
#include "formats/image/thirdparty/libamivideo/amivideotypes.h"
}

#include <cstring>

using ILBM_Image = formats::image::amiga::image;
bool ILBM_imageIsILBM(const ILBM_Image* image)
{
    return (image->type == formats::image::amiga::image_type::ILBM);
}

bool ILBM_imageIsACBM(const ILBM_Image* image)
{
    return (image->type == formats::image::amiga::image_type::ACBM);
}

bool ILBM_imageIsPBM(const ILBM_Image* image)
{
    return (image->type == formats::image::amiga::image_type::PBM);
}

unsigned int ILBM_calculateRowSize(const ILBM_Image* image)
{
    unsigned int rowSizeInWords = image->w / 16;

    if (image->w % 16 != 0)
    {
        rowSizeInWords++;
    }

    return (rowSizeInWords * 2);
}
unsigned int ILBM_calculateNumOfColors(const ILBM_Image* image)
{
    switch (image->nPlanes)
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

formats::image::rgb_palette ILBM_generateGrayscaleColorMap(const ILBM_Image* image)
{
    unsigned int numOfColors = ILBM_calculateNumOfColors(image);
    formats::image::rgb_palette colorMap(numOfColors);

    unsigned int i;

    for (i = 0; i < numOfColors; i++)
    {
        auto* colorRegister = &colorMap[i];
        unsigned int value = i * 0xff / (numOfColors - 1);

        colorRegister->r = value;
        colorRegister->g = value;
        colorRegister->b = value;
    }

    return colorMap;
}
// --------------------------------------------------------------------------------------------------------------------------
void SDL_ILBM_initPaletteFromImage(const ILBM_Image* image, amiVideo_Palette* palette)
{
    if (image->colors.empty())
    {
        /* If no colormap is provided by the image, use a generated grayscale one */
        auto colorMap = ILBM_generateGrayscaleColorMap(image);
        amiVideo_setBitplanePaletteColors(palette, (amiVideo_Color*) colorMap.data(), colorMap.size());
    } else
    {
        amiVideo_setBitplanePaletteColors(palette, (amiVideo_Color*) image->colors.data(),
                                          image->colors.size());
    } /* Otherwise, use the provided color map */
}

amiVideo_ULong SDL_ILBM_extractViewportModeFromImage(const ILBM_Image* image)
{
    amiVideo_ULong paletteFlags, resolutionFlags;

    if (!image->viewport_mode)
    {
        paletteFlags = 0; /* If no viewport value is set, assume 0 value */
    } else
    {
        paletteFlags = amiVideo_extractPaletteFlags(
                *image->viewport_mode);
    } /* Only the palette flags can be considered "reliable" from a viewport mode value */

    /* Resolution flags are determined by looking at the page dimensions */
    resolutionFlags = amiVideo_autoSelectViewportMode(image->pageWidth, image->pageHeight);

    /* Return the combined settings of the previous */
    return paletteFlags | resolutionFlags;
}

void ILBM_unpackByteRun(ILBM_Image* image)
{
    auto& body = image->body;

    /* Only perform decompression if the body is compressed and present */
    if (image->compression == formats::image::amiga::compression_type::BYTE_RUN && !body.empty())
    {
        /* Counters */
        unsigned int count = 0;
        unsigned int readBytes = 0;

        /* Allocate decompressed chunk attributes */

        auto chunkSize = ILBM_calculateRowSize(image) * image->h * image->nPlanes;
        std::vector<uint8_t> decompressedChunkData(chunkSize);

        /* Perform RLE decompression */

        while (readBytes < body.size())
        {
            int byte = (int8_t) body[readBytes];
            readBytes++;

            if (byte >= 0 && byte <= 127) /* Take the next byte bytes + 1 literally */
            {
                int i;

                for (i = 0; i < byte + 1; i++)
                {
                    decompressedChunkData[count] = body[readBytes];
                    readBytes++;
                    count++;
                }
            } else
            {
                if (byte >= -127 && byte <= -1) /* Replicate the next byte, -byte + 1 times */
                {
                    uint8_t ubyte;
                    int i;

                    ubyte = body[readBytes];
                    readBytes++;

                    for (i = 0; i < -byte + 1; i++)
                    {
                        decompressedChunkData[count] = ubyte;
                        count++;
                    }
                } else
                    RAISE_EX("Unknown byte run encoding byte!");
            }
        }

        /* Free the compressed chunk data */

        std::swap(image->body, decompressedChunkData);

        image->compression = formats::image::amiga::compression_type::NONE;
    }
}

void ILBM_deinterleaveToBitplaneMemory(const ILBM_Image* image, uint8_t** bitplanePointers)
{
    if (!image->body.empty())
    {
        unsigned int i;
        int count = 0; /* Offset in the interleaved source */
        int hOffset = 0; /* Horizontal offset in resulting bitplanes */
        unsigned int rowSize = ILBM_calculateRowSize(image);

        for (i = 0; i < image->h; i++)
        {
            unsigned int j;

            for (j = 0; j < image->nPlanes; j++)
            {
                memcpy(bitplanePointers[j] + hOffset, image->body.data() + count, rowSize);
                count += rowSize;
            }

            hOffset += rowSize;
        }
    }
}

void ILBM_deinterleave(ILBM_Image* image)
{
    auto nPlanes = image->nPlanes;
    unsigned int bitplaneSize = ILBM_calculateRowSize(image) * image->h;
    std::vector<uint8_t> result(bitplaneSize * nPlanes);

#define MAX_NUM_OF_BITPLANES 32

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
    ILBM_deinterleaveToBitplaneMemory(image, bitplanePointers);

    /* Return result */

    std::swap(image->body, result);
}

void SDL_ILBM_attachImageToScreen(ILBM_Image* image, amiVideo_Screen* screen)
{
    /* Determine which viewport mode is best for displaying the image */
    auto viewportMode = SDL_ILBM_extractViewportModeFromImage(image);

    /* Initialize the screen with the image's dimensions, bitplane depth, and viewport mode */
    amiVideo_initScreen(screen, image->w, image->h, image->nPlanes, 8, viewportMode);

    /* Sets the colors of the palette */
    SDL_ILBM_initPaletteFromImage(image, &screen->palette);

    /* Decompress the image body */
    ILBM_unpackByteRun(image);

    /* Attach the appropriate pixel surface to the screen */
    if (ILBM_imageIsPBM(image))
    {
        amiVideo_setScreenUncorrectedChunkyPixelsPointer(screen, (amiVideo_UByte*) image->body.data(),
                                                         image->w); /* A PBM has chunky pixels in its body */
    } else
    {
        if (ILBM_imageIsACBM(image))
        {
            amiVideo_setScreenBitplanes(screen,
                                        (amiVideo_UByte*) image->body.data()); /* Set bitplane pointers of the conversion screen */
        } else
        {
            if (ILBM_imageIsILBM(image))
            {
                /* Amiga ILBM image has interleaved scanlines per bitplane. We have to deinterleave it in order to be able to convert it */
                ILBM_deinterleave(image);
                amiVideo_setScreenBitplanes(screen,
                                            (amiVideo_UByte*) image->body.data()); /* Set bitplane pointers of the conversion screen */
            }
        }
    }
}

static constexpr uint32_t Rmask = 0x00FF0000;
static constexpr uint32_t Gmask = 0x0000FF00;
static constexpr uint32_t Bmask = 0x000000FF;
static constexpr uint32_t Amask = 0xFF000000;

int eval_mask(uint32_t xmask)
{
    int shift = 0;
    for (uint32_t mask = xmask; !(mask & 0x01); mask >>= 1)
    {
        ++shift;
    }
    return shift;
}


static const uint32_t Rshift = eval_mask(Rmask);
static const uint32_t Gshift = eval_mask(Gmask);
static const uint32_t Bshift = eval_mask(Bmask);
static const uint32_t Ashift = eval_mask(Amask);

struct Surface
{

    Surface(unsigned int w, unsigned int h, int depth)
            : pitch(w),
              height(h),
              bit_depth(depth)
    {
        if (depth == 8)
        {
            pixels.resize(w * h);
        } else
        {
            pixels.resize(w * h * sizeof(uint32_t));
        }
    }

    [[nodiscard]] formats::image::picture convert() const
    {
        formats::image::picture out(pitch, height, bit_depth == 32);
        if (bit_depth == 8)
        {
            formats::image::indexed_color_mapper m(palette);
            for (unsigned int y = 0; y < height; y++)
            {
                for (unsigned int x=0; x < pitch; x++)
                {
                    uint8_t color = pixels[y*pitch+x];
                    out.put(x, y, color, m);
                }
            }
        }
        else
        {
            const auto* dwords = (uint32_t*)pixels.data();
            for (unsigned int y = 0; y < height; y++)
            {
                for (unsigned int x=0; x < pitch; x++)
                {
                    uint32_t color = dwords[y*pitch+x];
                    uint8_t r = (color & Rmask) >> Rshift;
                    uint8_t g = (color & Gmask) >> Gshift;
                    uint8_t b = (color & Bmask) >> Bshift;
                    uint8_t a = (color & Amask) >> Ashift;
                    out.put(x, y, r, g, b, a);
                }
            }
        }
        return out;
    }

    unsigned int pitch;
    unsigned int height;
    int bit_depth;
    std::vector<uint8_t> pixels;
    formats::image::rgba_palette palette;
};

void SDL_ILBM_setSurfacePaletteFromScreenPalette(amiVideo_Palette* palette, Surface& surface)
{
    for (unsigned int i = 0; i < palette->chunkyFormat.numOfColors; i++)
    {
        const auto& x = palette->chunkyFormat.color[i];
        formats::image::rgba c{x.r, x.g, x.b, x.a};
        surface.palette.push_back(c);
    }
}

Surface* SDL_ILBM_createUncorrectedChunkySurfaceFromScreen(amiVideo_Screen* screen)
{
    auto* surface = new Surface(screen->width, screen->height, 8);

    /* Sets the uncorrected chunky pixels pointer of the conversion struct to that of the SDL pixel surface */
    amiVideo_setScreenUncorrectedChunkyPixelsPointer(screen, surface->pixels.data(), surface->pitch);

    /* Convert the colors of the the bitplane palette to the format of the chunky palette */
    amiVideo_convertBitplaneColorsToChunkyFormat(&screen->palette);

    /* Set the palette of the target SDL surface */
    SDL_ILBM_setSurfacePaletteFromScreenPalette(&screen->palette, *surface);

    return surface;
}


Surface* SDL_ILBM_createUncorrectedRGBSurfaceFromScreen(amiVideo_Screen* screen, const ILBM_Image* image)
{
    auto* surface = new Surface(screen->width, screen->height, 32);
    int allocateUncorrectedMemory = !ILBM_imageIsPBM(image);
    /* Set the uncorrected RGB pixels pointer of the conversion struct to that of the SDL pixel surface */
    amiVideo_setScreenUncorrectedRGBPixelsPointer(screen, (amiVideo_ULong*) surface->pixels.data(), surface->pitch,
                                                  allocateUncorrectedMemory, Rshift, Gshift, Bshift, Ashift);


    return surface;
}

void SDL_ILBM_renderUncorrectedChunkyImage(const ILBM_Image* image, amiVideo_Screen* screen)
{
    if (ILBM_imageIsPBM(image))
    {
        memcpy(screen->uncorrectedChunkyFormat.pixels, image->body.data(),
               image->body.size()); /* For a PBM no conversion is needed => simply copy the data into the surface */
    } else
    {
        amiVideo_convertScreenBitplanesToChunkyPixels(screen); /* Convert the bitplanes to chunky pixels */
    }
}

void SDL_ILBM_renderUncorrectedRGBImage(const ILBM_Image* image, amiVideo_Screen* screen)
{
    if (ILBM_imageIsPBM(image))
    {
        if (screen->bitplaneDepth == 24 || screen->bitplaneDepth == 32)
        {
            /* For images with a higher than 8 bitplane depth (true color images) => copy chunky data to the RGB section and reorder the pixels */
            memcpy(screen->uncorrectedRGBFormat.pixels, image->body.data(), image->body.size());
            amiVideo_reorderRGBPixels(screen);
        } else
        {
            amiVideo_convertScreenChunkyPixelsToRGBPixels(screen);
        } /* Convert chunky to RGB data */

    } else
    {
        amiVideo_convertScreenBitplanesToRGBPixels(screen); /* Convert the bitplanes to RGB pixels */
    }
}

static Surface* createSurfaceFromScreen(amiVideo_Screen* screen, ILBM_Image* image)
{
    /* Attach the image to screen conversion pipeline */
    SDL_ILBM_attachImageToScreen(image, screen);

    auto realFormat = amiVideo_autoSelectColorFormat(screen);

    /* Create and render the surface */
    Surface* surface = nullptr;
    if (realFormat == AMIVIDEO_CHUNKY_FORMAT)
    {
        surface = SDL_ILBM_createUncorrectedChunkySurfaceFromScreen(screen);
        SDL_ILBM_renderUncorrectedChunkyImage(image, screen);
    } else
    {
        surface = SDL_ILBM_createUncorrectedRGBSurfaceFromScreen(screen, image);
        SDL_ILBM_renderUncorrectedRGBImage(image, screen);
    }

    return surface;
}

namespace formats::image::amiga
{
#define ILBM_MSK_NONE 0
#define ILBM_MSK_HAS_MASK 1
#define ILBM_MSK_HAS_TRANSPARENT_COLOR 2
#define ILBM_MSK_LASSO  3
    // ============================================================================================================
    void image::convert(formats::image::picture& out)
    {
        amiVideo_Screen screen;
        SDL_ILBM_attachImageToScreen(this, &screen);
        Surface* surface = createSurfaceFromScreen(&screen, this);
        out = surface->convert();
        delete surface;
        amiVideo_cleanupScreen(&screen);
    }
    // ============================================================================================================
    void image::update(formats::iff::chunk_type chunk_type, std::istream& is, std::size_t size)
    {
        using namespace formats::amiga;
        std::visit(bsw::overload(
                [this](const bmhd& e) {
                    w = e.Width;
                    h = e.Height;
                    nPlanes = e.Bitplanes;
                    masking = masking_type::NONE;
                    switch (e.Masking)
                    {
                        case ILBM_MSK_HAS_MASK:
                            masking = masking_type::HAS_MASK;
                            break;
                        case ILBM_MSK_NONE:
                            masking = masking_type::NONE;
                            break;
                        case ILBM_MSK_HAS_TRANSPARENT_COLOR:
                            masking = masking_type::HAS_TRANSPARENT_COLOR;
                            break;
                        case ILBM_MSK_LASSO:
                            masking = masking_type::LASSO;
                            break;
                        default:
                            RAISE_EX("Unknown masking type ", (int) e.Masking);
                    }
                    if (e.Compress > 1)
                    {
                        RAISE_EX("Unknown compression type ", (int) e.Compress);
                    } else
                    {
                        compression = (e.Compress == 1) ? compression_type::BYTE_RUN : compression_type::NONE;
                    }
                    pageWidth = e.PageWidth;
                    pageHeight = e.Height;
                },
                           // --------------------------------------------------------
                [this](const vport& e) {
                    viewport_mode = e.mode;
                },
                           // --------------------------------------------------------
                [this](const cmap& e) {
                    colors.resize(e.colors.size());
                    std::memcpy(colors.data(), e.colors.data(), e.colors.size());
                },
                           // --------------------------------------------------------
                [this](const formats::amiga::body& e) {
                    body = e.data;
                },
                [this](const bitplanes& e) {
                    body = e.data;
                },
                           // --------------------------------------------------------
                [](const std::monostate&) {}
                   ),
                   parse_chunk<bmhd, vport, cmap, formats::amiga::body, bitplanes>(chunk_type, is, size));
    }
    // ========================================================================================================
    image::image(image_type t)
            : type(t)
    {

    }
}