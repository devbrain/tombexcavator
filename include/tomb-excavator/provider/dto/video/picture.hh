//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_DTO_PICTURE_HH
#define TOMBEXCAVATOR_DTO_PICTURE_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/dto/video/palette.hh>
#include <tomb-excavator/provider/dto/video/canvas.hh>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace provider::dto
{
    class PROVIDER_API picture
    {
    public:
        picture(int x, int y, const palette& pal);
        picture(const dimension& dim, const palette& pal);

        [[nodiscard]] const dimension& dim() const noexcept;

        void fill (unsigned char color);

        void put(int x, int y, unsigned char pixel);
        [[nodiscard]] unsigned char get(int x, int y) const;

        [[nodiscard]] const palette& pal () const noexcept;

        [[nodiscard]] canvas& get_canvas() noexcept;
        [[nodiscard]] const canvas& get_canvas() const noexcept;
    private:
        canvas m_canvas;
        palette m_pal;
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_PICTURE_HH
