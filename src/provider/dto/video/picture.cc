//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/provider/dto/video/picture.hh>

namespace provider::dto
{
    picture::picture(int x, int y, const palette& pal)
    : m_canvas(x, y),
    m_pal(pal)
    {

    }
    // -----------------------------------------------------------------------------------------------
    picture::picture(const dimension& dim, const palette& pal)
    : m_canvas(dim),
    m_pal (pal)
    {

    }
    // -----------------------------------------------------------------------------------------------
    const dimension& picture::dim() const noexcept
    {
        return m_canvas.dim();
    }
    // -----------------------------------------------------------------------------------------------
    void picture::fill(unsigned char color)
    {
        m_canvas.fill(color);
    }
    // -----------------------------------------------------------------------------------------------
    void picture::put(int x, int y, unsigned char pixel)
    {
        m_canvas.put(x, y, pixel);
    }
    // -----------------------------------------------------------------------------------------------
    unsigned char picture::get(int x, int y) const
    {
        return m_canvas.get(x, y);
    }
    // -----------------------------------------------------------------------------------------------
    const palette& picture::pal() const noexcept
    {
        return m_pal;
    }
    // -----------------------------------------------------------------------------------------------
    canvas& picture::get_canvas() noexcept
    {
        return m_canvas;
    }
    // -----------------------------------------------------------------------------------------------
    const canvas& picture::get_canvas() const noexcept
    {
        return m_canvas;
    }
}