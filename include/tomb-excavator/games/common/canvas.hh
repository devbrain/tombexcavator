//
// Created by igor on 27/05/2020.
//

#ifndef TOMBEXCAVATOR_CANVAS_HH
#define TOMBEXCAVATOR_CANVAS_HH

#include <vector>
#include <string>

#include <tomb-excavator/games-common_export.h>
#include "tomb-excavator/games/common/coord.hh"
#include "tomb-excavator/games/common/palette.hh"

namespace common
{
    class GAMES_COMMON_API canvas
    {
    public:
        canvas();
        canvas(int w, int h);
        canvas(const dimension& dim);

        [[nodiscard]] palette& pal() noexcept ;
        [[nodiscard]] const palette& pal() const noexcept ;

        [[nodiscard]] const dimension& dim() const noexcept;
        [[nodiscard]] bool empty() const noexcept ;
        [[nodiscard]] std::size_t size() const noexcept ;

        void fill (unsigned char color);

        void copy(int x, int y, const std::vector<unsigned char>& src, int src_w, int src_h);
        void put(int x, int y, unsigned char pixel);

        [[nodiscard]] unsigned char* data() noexcept ;
        [[nodiscard]] const unsigned char* data() const noexcept ;
    private:
        palette m_pal;
        dimension m_dim;
        std::vector<unsigned char> m_pixels;
    };
}

#endif //TOMBEXCAVATOR_CANVAS_HH
