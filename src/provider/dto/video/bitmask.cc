//
// Created by igor on 24/02/2021.
//

#include <tomb-excavator/provider/dto/video/bitmask.hh>

namespace provider::dto
{
    bitmask::bitmask(int w, int h)
    : m_dim(w, h),
      m_mask(w*h, false)
    {

    }
    // -------------------------------------------------------------------------------------
    bitmask::bitmask(const dimension& dim)
            : m_dim(dim),
              m_mask(dim.w() * dim.h(), false)
    {

    }
    // -------------------------------------------------------------------------------------
    void bitmask::set(int x, int y)
    {
        m_mask[m_dim.w()*y + x] = true;
    }
    // -------------------------------------------------------------------------------------
    bool bitmask::is_set(int x, int y) const
    {
        return m_mask[m_dim.w()*y + x];
    }
    // -------------------------------------------------------------------------------------
    const dimension& bitmask::dim() const noexcept
    {
        return m_dim;
    }
}
