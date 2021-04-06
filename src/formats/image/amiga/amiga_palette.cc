//
// Created by igor on 06/04/2021.
//

#include "amiga_palette.hh"

namespace formats::image::amiga
{
    bitplane_format_colors::bitplane_format_colors(unsigned int bitplaneDepth, unsigned int bitsPerColorChannel, viewport_mode viewportMode)
    : m_viewport(viewportMode),
      m_colors(num_of_colors(bitplaneDepth)),
      m_bits_per_channel(bitsPerColorChannel)
    {

    }
    // -----------------------------------------------------------------------------------
    int bitplane_format_colors::num_of_colors(unsigned int bitplaneDepth) noexcept
    {
        switch(bitplaneDepth)
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
    // -----------------------------------------------------------------------------------
    std::vector<uint16_t> bitplane_format_colors::generate_rgb4_color_specs() const
    {
        unsigned shift = m_bits_per_channel - 4;
        std::vector<uint16_t> colorSpecs;
        colorSpecs.reserve(m_colors.size());

        for(const auto& color : m_colors)
        {
            const uint32_t r = ((uint16_t)color.r & 0xFFu) >> shift;
            const uint32_t g = ((uint16_t)color.g & 0xFFu) >> shift;
            const uint32_t b = ((uint16_t)color.b & 0xFFu) >> shift;
            const auto c = (uint16_t)(((r << 8u) | (g << 4u) | b) & 0xFFFFu);
            colorSpecs.push_back(c);
        }

        return colorSpecs;
    }
    // -----------------------------------------------------------------------------------
    std::vector<uint32_t> bitplane_format_colors::generate_rgb32_color_specs() const
    {
        unsigned int index = 1;
        unsigned shift = 32 - m_bits_per_channel;
        std::vector<uint32_t> colorSpecs(m_colors.size() * 3 + 2);

        /* First element's first word is number of colors, second word is the first color to be loaded (which is 0) */
        colorSpecs[0] = m_colors.size() << 16u;

        /* Remaining elements are red, green, blue component values for each color register */
        for (const auto& color : m_colors)
        {
            colorSpecs[index++] = color.r << shift;
            colorSpecs[index++] = color.g << shift;
            colorSpecs[index++] = color.b << shift;
        }

        /* Add 0 termination at the end */
        colorSpecs[index] = 0;

        /* Return the generated color specs */
        return colorSpecs;
    }
    // ------------------------------------------------------------------------------------------------
    const rgb_palette& bitplane_format_colors::palette() const noexcept
    {
        return m_colors;
    }
    // ------------------------------------------------------------------------------------------------
    void bitplane_format_colors::convert(const chunky_format_colors& x)
    {
        unsigned shift = 8 - m_bits_per_channel;

        for(std::size_t i = 0; i < x.m_colors.size(); i++)
        {
            const auto& chunkyColor = m_colors[i];
            auto& color = m_colors[i];

            color.r = chunkyColor.r >> shift;
            color.g = chunkyColor.g >> shift;
            color.b = chunkyColor.g >> shift;
        }
    }
    // ================================================================================================
    chunky_format_colors::chunky_format_colors(const bitplane_format_colors& x)
    {
        auto numOfColors = x.m_colors.size();
        const auto extra_half_bright = x.m_viewport.is_extra_half_bright();
        if (extra_half_bright)
        {
            numOfColors = 2 * numOfColors; /* Extra halfbrite screen mode has double the amount of colors */
        }
        m_colors.resize(numOfColors);
        /* We must convert color channels that consist do not consist of 8 bits */

        unsigned int shift = 8 - x.m_bits_per_channel;
        auto num_colors = x.m_colors.size();
        for(std::size_t i = 0; i < num_colors; i++)
        {
            const auto& sourceColor = x.m_colors[i];
            auto& targetColor = m_colors[i];

            targetColor.r = sourceColor.r << shift;
            targetColor.g = sourceColor.g << shift;
            targetColor.b = sourceColor.b << shift;
            targetColor.a = '\0';

            if (extra_half_bright)
            {
                /* For extra half brite screen modes we must append half of the color values of the original color register values */
                targetColor = m_colors[i + x.m_colors.size()];

                targetColor.r = sourceColor.r >> 1u;
                targetColor.g = sourceColor.g >> 1u;
                targetColor.b = sourceColor.b >> 1u;
                targetColor.a = '\0';
            }
        }
    }
    // ------------------------------------------------------------------------------------------------
    chunky_format_colors::chunky_format_colors (unsigned int bitplaneDepth, viewport_mode viewportMode)
    {
        auto numOfColors = bitplane_format_colors::num_of_colors(bitplaneDepth);
        if (viewportMode.is_extra_half_bright())
        {
            numOfColors = 2 * numOfColors; /* Extra halfbrite screen mode has double the amount of colors */
        }
        m_colors.resize(numOfColors);
    }
    // ------------------------------------------------------------------------------------------------
    const rgba_palette& chunky_format_colors::palette() const noexcept
    {
        return m_colors;
    }
}
