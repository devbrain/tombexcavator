//
// Created by igor on 06/04/2021.
//

#ifndef TOMBEXCAVATOR_AMIGA_PALETTE_HH
#define TOMBEXCAVATOR_AMIGA_PALETTE_HH

#include <vector>
#include <cstdint>
#include <tomb-excavator/formats/image/picture.hh>
#include "formats/image/amiga/amiga_viewport.hh"

namespace formats::image::amiga
{
    class chunky_format_colors;
    class bitplane_format_colors
    {
        friend class chunky_format_colors;
    public:
        bitplane_format_colors(unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, viewport_mode viewportMode);

        [[nodiscard]] static int num_of_colors(unsigned int bitplaneDepth) noexcept;
        /**
         * Converts the bitplane palette to an array of word specifications, which can
         * be used by AmigaOS' LoadRGB4() function to set a screen's palette. This
         * function does not support the AGA chipset's capabilities. The resulting array
         * has as many elements as the bitplane palette
         *
         * @return A word array containing the color specifications for the LoadRGB4() function.
         */
        [[nodiscard]] std::vector<uint16_t> generate_rgb4_color_specs() const;

        /**
         * Converts the bitplane palette to an array of long integer color
         * specifications, which can be used by AmigaOS' LoadRGB32() function to set a
         * screen's palette. To use the AGA chipset's abilities, it's required to use
         * this function.
         *
         * @return An long integer array containing the color specifications for the LoadRGB32() function.
        */
        [[nodiscard]] std::vector<uint32_t> generate_rgb32_color_specs() const;

        [[nodiscard]] const rgb_palette& palette() const noexcept;

        /**
         * Converts the palette used for chunky graphics to a format that can be for
         * displaying bitplanes. If an palette with 4 bit color components is used,
         * then the color components are shifted.
         *
         * @param palette Palette conversion structure
        */
        void convert(const chunky_format_colors& x);

    private:
        viewport_mode m_viewport;
        rgb_palette  m_colors;
        /** Contains the number of bits that a color component has (4 = OCS/ECS, 8 = AGA) */
        unsigned int m_bits_per_channel;
    };
    // ==========================================================================================================
    class chunky_format_colors
    {
        friend class bitplane_format_colors;
    public:
        explicit chunky_format_colors(const bitplane_format_colors& x);
        chunky_format_colors (unsigned int bitplaneDepth, viewport_mode viewportMode);

        [[nodiscard]] const rgba_palette& palette() const noexcept;
    private:
        rgba_palette  m_colors;
    };


}

#endif //TOMBEXCAVATOR_AMIGA_PALETTE_HH
