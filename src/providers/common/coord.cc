//
// Created by igor on 27/05/2020.
//

#include "tomb-excavator/games/common/coord.hh"

namespace common
{
    position::position(int x, int y)
    : m_x(x), m_y(y)
    {

    }
    int position::x() const noexcept
    {
        return m_x;
    }
    int position::y() const noexcept
    {
        return m_y;
    }
    // =============================================================
    dimension::dimension(int w, int h)
            : m_w(w), m_h(h)
    {

    }
    int dimension::w() const noexcept
    {
        return m_w;
    }
    int dimension::h() const noexcept
    {
        return m_h;
    }
    // ============================================================
    coord::coord(const position& p, const dimension& d)
            : m_pos(p), m_dim(d)
    {

    }
    const position& coord::pos() const noexcept
    {
        return m_pos;
    }
    const dimension& coord::dim() const noexcept
    {
        return m_dim;
    }
}