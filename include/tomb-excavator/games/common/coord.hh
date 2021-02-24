//
// Created by igor on 27/05/2020.
//

#ifndef COMMON_COORD_HH
#define COMMON_COORD_HH

#include <tombexcavator-games-common_export.h>

namespace common
{
    class GAMES_COMMON_API position
    {
    public:
        position(int x, int y);
        int x() const noexcept ;
        int y() const noexcept ;
    private:
        int m_x;
        int m_y;
    };

    class GAMES_COMMON_API dimension
    {
    public:
        dimension(int w, int h);
        int w() const noexcept ;
        int h() const noexcept ;
    private:
        int m_w;
        int m_h;
    };

    class GAMES_COMMON_API coord
    {
    public:
        coord(const position& p, const dimension& d);
        const position& pos() const noexcept ;
        const dimension& dim () const noexcept ;
    private:
        position m_pos;
        dimension m_dim;
    };
}

#endif

