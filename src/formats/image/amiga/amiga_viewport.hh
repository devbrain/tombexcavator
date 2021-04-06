//
// Created by igor on 06/04/2021.
//

#ifndef TOMBEXCAVATOR_AMIGA_VIEWPORT_HH
#define TOMBEXCAVATOR_AMIGA_VIEWPORT_HH

#include <cstdint>

namespace formats::image::amiga
{
    class viewport_mode
    {
    public:
        static constexpr uint32_t LACE       = 0x0004u;
        static constexpr uint32_t EHB        = 0x0080u;
        static constexpr uint32_t HAM        = 0x0800u;
        static constexpr uint32_t HIRES      = 0x8000u;
        static constexpr uint32_t SUPERHIRES = 0x8020u;
    public:
        explicit viewport_mode(uint32_t mode);

        viewport_mode(const viewport_mode&) = default;
        viewport_mode& operator = (const viewport_mode&) = default;

        // Checks whether the Extra-Halfbrite (EHB) bit is enabled in the viewport mode register.
        [[nodiscard]] bool is_extra_half_bright() const noexcept;

        // Checks whether the Hold-and-Modify (HAM) bit is enabled in the viewport mode register
        [[nodiscard]] bool is_hold_and_modify() const noexcept;

        // Checks whether the hires bit is enabled in the viewport mode register.
        [[nodiscard]] bool is_hi_res() const noexcept;

        // Checks whether the super and hires bits are enabled in the viewport mode
        // register.
        [[nodiscard]] bool is_super_hires() const noexcept;

        // Checks whether the hires bit is enabled in the viewport mode register.
        [[nodiscard]] bool is_laced() const noexcept;

        /**
         * Auto selects the most space efficient lowres pixel scale factor capable of
         * retaining the right aspect ratio on non-Amiga displays.
         *
         * @return The most efficient lowres pixel scale factor
        */
        [[nodiscard]] uint32_t select_lowres_pixel_scale_factor() const noexcept;

        /**
        * Extracts the palette flag values bits (Extra Half Brite and Hold-and Modify)
        * from the viewport mode value.
        *
        * @return A viewport mode value with only the EHB and HAM flags set
        */
        [[nodiscard]] uint32_t extra_palette_flags() const noexcept;

        /**
         * Auto selects the most suitable Amiga resolution viewport flags to display a
         * given screen.
         *
         * @param width Width of the screen
         * @param height Height of the screen
         * @return A viewport mode value with the most suitable resolution flags set
         */
        [[nodiscard]] static viewport_mode auto_select_vieport_mode(unsigned width, unsigned height) noexcept;
    private:
        uint32_t m_mode;
    };
}

#endif //TOMBEXCAVATOR_AMIGA_VIEWPORT_HH
