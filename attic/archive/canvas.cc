//
// Created by igor on 27/05/2020.
//

#include "canvas.hh"
#include "output.hh"
#include <cstring>
#include <algorithm>

namespace archive
{
    canvas::canvas()
    : m_dim(0,0)
    {

    }
    // ---------------------------------------------------------------------------
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
    palette& canvas::pal() noexcept
    {
        return m_pal;
    }
    // ---------------------------------------------------------------------------
    const palette& canvas::pal() const noexcept
    {
        return m_pal;
    }
    // ---------------------------------------------------------------------------
    const dimension& canvas::dim() const noexcept
    {
        return m_dim;
    }
    // ---------------------------------------------------------------------------
    bool canvas::empty() const noexcept
    {
        return m_pixels.empty();
    }
    // ---------------------------------------------------------------------------
    std::size_t canvas::size() const noexcept
    {
        return m_pixels.size();
    }
    // ---------------------------------------------------------------------------
    void canvas::fill (unsigned char color)
    {
        std::memset(m_pixels.data(), m_pixels.size(), color);
    }
    // ---------------------------------------------------------------------------
    unsigned char* canvas::data() noexcept
    {
        return m_pixels.data();
    }
    // ---------------------------------------------------------------------------
    const unsigned char* canvas::data() const noexcept
    {
        return m_pixels.data();
    }
    // ---------------------------------------------------------------------------
    void canvas::copy(int x, int y, const std::vector<unsigned char>& src, int src_w, int src_h)
    {
        auto w = std::min(m_dim.w() - x, src_w);
        auto h = std::min(m_dim.h() - y, src_h);
        for (int py = 0; py<h; py++)
        {
            for (int px=0; px<w; px++)
            {
                auto dst_idx = m_dim.w()*(y+py) + (x+px);
                auto src_idx = src_w*py + px;
                m_pixels[dst_idx] = src[src_idx];
            }
        }
    }
    // ---------------------------------------------------------------------------
    bool canvas::save_to_png(const std::string& filename) const
    {
        return save_pixels(filename, m_pixels.data(), m_dim.w(), m_dim.h(), m_pal);
    }
}