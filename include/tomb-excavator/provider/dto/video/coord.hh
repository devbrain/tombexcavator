//
// Created by igor on 27/05/2020.
//

#ifndef COMMON_COORD_HH
#define COMMON_COORD_HH

#include <tomb-excavator/export-provider.h>

namespace provider::dto
{
    class PROVIDER_API position
    {
    public:
        position(int x, int y);
        [[nodiscard]] int x() const noexcept ;
        [[nodiscard]] int y() const noexcept ;
    private:
        int m_x;
        int m_y;
    };

    class PROVIDER_API dimension
    {
    public:
        dimension(int w, int h);
        [[nodiscard]] int w() const noexcept ;
        [[nodiscard]] int h() const noexcept ;
    private:
        int m_w;
        int m_h;
    };

    class PROVIDER_API coord
    {
    public:
        coord(const position& p, const dimension& d);
        [[nodiscard]] const position& pos() const noexcept ;
        [[nodiscard]] const dimension& dim () const noexcept ;
    private:
        position m_pos;
        dimension m_dim;
    };
}

#endif

