//
// Created by igor on 21/03/2021.
//

#ifndef TOMBEXCAVATOR_CANVAS_UTILS_HH
#define TOMBEXCAVATOR_CANVAS_UTILS_HH

#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/provider/dto/video/sprite.hh>
#include <tomb-excavator/export/romfont.hh>

namespace exporter
{
    provider::dto::coord draw_on_canvas(formats::image::picture& canvas, int x, int y, const provider::dto::sprite& sprite,
                                        const provider::dto::palette& pal);

    provider::dto::coord draw_on_canvas(formats::image::picture& canvas, int x, int y, font fnt, const std::string& str,
                                        const provider::dto::rgba& color);

    provider::dto::coord draw_on_canvas(formats::image::picture& canvas, int x, int y, font fnt, char ch,
                                        const provider::dto::rgba& color);
}

#endif //TOMBEXCAVATOR_CANVAS_UTILS_HH
