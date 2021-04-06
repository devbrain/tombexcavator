//
// Created by igor on 05/04/2021.
//

#ifndef TOMBEXCAVATOR_AMIGA_UTILS_HH
#define TOMBEXCAVATOR_AMIGA_UTILS_HH

#include <cstdint>


struct amiVideo_Screen;

namespace formats::image::amiga
{
    struct image;
    struct bmhd_entry;


    void attach_image_to_screen(image* image, amiVideo_Screen* screen);
}


#endif //TOMBEXCAVATOR_AMIGA_UTILS_HH
