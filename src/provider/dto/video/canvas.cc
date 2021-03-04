//
// Created by igor on 27/05/2020.
//

#include "tomb-excavator/provider/dto/video//canvas.hh"

#include <cstring>
namespace provider::dto
{
    canvas::canvas(int w, int h)
    : m_dim{w,h}
    {
        m_pixels.resize(w*h, 0);
    }
    // ---------------------------------------------------------------------------
    canvas::canvas(const dimension& dim)
    : m_dim(dim)
    {
        m_pixels.resize(m_dim.w()*m_dim.h(), 0);
    }
    // ---------------------------------------------------------------------------
    const dimension& canvas::dim() const noexcept
    {
        return m_dim;
    }
    // ---------------------------------------------------------------------------
    void canvas::fill (unsigned char color)
    {
        std::memset(m_pixels.data(), m_pixels.size(), color);
    }
    // ---------------------------------------------------------------------------
    void canvas::put(int x, int y, unsigned char pixel)
    {
        const auto dst_idx = m_dim.w()*y + x;
        m_pixels[dst_idx] = pixel;
    }
    // ---------------------------------------------------------------------------
    unsigned char canvas::get(int x, int y) const
    {
        const auto dst_idx = m_dim.w()*y + x;
        return m_pixels[dst_idx];
    }
}