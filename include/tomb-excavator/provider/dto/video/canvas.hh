//
// Created by igor on 27/05/2020.
//

#ifndef TOMBEXCAVATOR_CANVAS_HH
#define TOMBEXCAVATOR_CANVAS_HH

#include <vector>
#include <string>

#include <tomb-excavator/export-provider.h>
#include "tomb-excavator/provider/dto/video/coord.hh"
#include "tomb-excavator/provider/dto/video/palette.hh"
#include <tomb-excavator/msvc/c4251-begin.h>

namespace provider::dto
{
    class PROVIDER_API canvas
    {
    public:
        canvas(int w, int h);
        explicit canvas(const dimension& dim);

        [[nodiscard]] const dimension& dim() const noexcept;

        void fill (unsigned char color);

        void put(int x, int y, unsigned char pixel);
        [[nodiscard]] unsigned char get(int x, int y) const;

    private:
        dimension m_dim;
        std::vector<unsigned char> m_pixels;
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_CANVAS_HH
