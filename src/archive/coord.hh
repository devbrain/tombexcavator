//
// Created by igor on 27/05/2020.
//

#ifndef ARCHIVE_COORD_HH
#define ARCHIVE_COORD_HH

#include <archive/archive_api.h>

namespace archive
{
    class ARCHIVE_API position
    {
    public:
        position(int x, int y);
        int x() const noexcept ;
        int y() const noexcept ;
    private:
        int m_x;
        int m_y;
    };

    class ARCHIVE_API dimension
    {
    public:
        dimension(int w, int h);
        int w() const noexcept ;
        int h() const noexcept ;
    private:
        int m_w;
        int m_h;
    };

    class ARCHIVE_API coord
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

#endif //TOMBEXCAVATOR_COORD_HH
