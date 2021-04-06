//
// Created by igor on 06/04/2021.
//

#include "amiga_viewport.hh"

namespace formats::image::amiga
{
    viewport_mode::viewport_mode(uint32_t mode)
            : m_mode(mode)
    {

    }
    // ----------------------------------------------------------------------------------------------------
    bool viewport_mode::is_extra_half_bright() const noexcept
    {
        return ((m_mode & EHB) == EHB);
    }
    // ----------------------------------------------------------------------------------------------------
    bool viewport_mode::is_hold_and_modify() const noexcept
    {
        return ((m_mode & HAM) == HAM);
    }
    // ----------------------------------------------------------------------------------------------------
    bool viewport_mode::is_hi_res() const noexcept
    {
        return ((m_mode & HIRES) == HIRES);
    }
    // ----------------------------------------------------------------------------------------------------
    bool viewport_mode::is_super_hires() const noexcept
    {
        return ((m_mode & SUPERHIRES) == SUPERHIRES);
    }
    // ----------------------------------------------------------------------------------------------------
    bool viewport_mode::is_laced() const noexcept
    {
        return ((m_mode & LACE) == LACE);
    }
    // ----------------------------------------------------------------------------------------------------
    unsigned int viewport_mode::select_lowres_pixel_scale_factor() const noexcept
    {
        if (is_super_hires())
        {
            return 4;
        } else
        {
            if (is_hi_res() && is_laced())
            {
                return 1;
            } else
            {
                if (is_hi_res() && !is_laced())
                {
                    return 2;
                } else
                {
                    if (is_laced())
                    {
                        return 2;
                    } else
                    {
                        return 1;
                    }
                }
            }
        }
    }
    // ----------------------------------------------------------------------------------------------------
    uint32_t viewport_mode::extra_palette_flags() const noexcept
    {
        return m_mode & (HAM | EHB);
    }
    // ----------------------------------------------------------------------------------------------------
    viewport_mode viewport_mode::auto_select_vieport_mode(unsigned width, unsigned height) noexcept
    {
        uint32_t mode = 0;

        if (width > 736)
        {
            mode |= SUPERHIRES; /* If the page width is larger than 736 (640 width + max overscan), we use super hi-res screen mode */
        } else
        {
            if (width > 368)
            {
                mode |= HIRES;
            }
        } /* If the page width is larger than 368 (320 width + max overscan), we use hi-res screen mode */

        /* If the page height is larger than 290 (256 height + max overscan), we have a laced screen mode */
        if (height > 290)
            mode |= LACE;

        return viewport_mode{mode};
    }
}
