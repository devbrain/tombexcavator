//
// Created by igor on 14/02/2021.
//

#include <cstring>

#include "tomb-excavator/formats/image/jpeg.hh"
#include "tomb-excavator/formats/image/tga.hh"
#include "tomb-excavator/formats/image/bmp.hh"
#include "tomb-excavator/formats/image/png.hh"
#include "tomb-excavator/formats/image/psd.hh"
#include "tomb-excavator/formats/image/gif.hh"

#include "formats/image/picture_loader.hh"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

typedef int  (*tester_fn_ptr) (stbi__context *s, int *x, int *y, int *comp);

static bool stbi_wrapper(tester_fn_ptr tester, const char* input, std::size_t input_length, formats::image::picture& out)
{
    stbi__context s;
    stbi__start_mem (&s, (stbi_uc*)input, input_length);
    int x;
    int y;
    int comp;

    if (tester(&s, &x, &y, &comp))
    {
        int bppToUse = (comp == STBI_grey || comp == STBI_rgb) ? STBI_rgb : STBI_rgb_alpha;
        out.height = y;
        out.width = x;
        out.bpp = (bppToUse == STBI_rgb) ? 3 : 4;
        void* data = stbi_load_from_memory((stbi_uc*)input, input_length, &x, &y, &comp, bppToUse);
        out.pixels.resize(out.height*out.width*out.bpp);
        std::memcpy(out.pixels.data(), data, out.pixels.size());
        STBI_FREE(data);
        return true;
    }
    return false;
}
#define CONCATENATE_DIRECT(s1,s2) s1##s2
#define CONCATENATE(s1,s2) CONCATENATE_DIRECT(s1,s2)

#define DECLARE_STBI_METHOD(EXT)                                                                    \
bool CONCATENATE(is_,EXT)(const char* input, std::size_t input_length)                              \
{                                                                                                   \
        stbi__context s;                                                                            \
        stbi__start_mem (&s, (stbi_uc*)input, input_length);                                        \
        return CONCATENATE(stbi__, CONCATENATE(EXT,_test))(&s);                                     \
}                                                                                                   \
                                                                                                    \
bool CONCATENATE(load_,EXT)(const char* input, std::size_t input_length, picture& out)              \
{                                                                                                   \
    return stbi_wrapper(CONCATENATE(stbi__, CONCATENATE(EXT,_info)), input, input_length, out);     \
}                                                                                                   \
                                                                                                    \
struct CONCATENATE(register_fn,__LINE__)                                                            \
{                                                                                                   \
    CONCATENATE(register_fn,__LINE__)() noexcept                                                    \
    {                                                                                               \
       picture_loader::instance().register_functions(CONCATENATE(is_,EXT), CONCATENATE(load_,EXT)); \
    }                                                                                               \
};                                                                                                  \
static CONCATENATE(register_fn,__LINE__)  CONCATENATE(CONCATENATE(register_fn,__LINE__),_inst)

namespace formats::image
{
    DECLARE_STBI_METHOD(jpeg);
    DECLARE_STBI_METHOD(tga);
    DECLARE_STBI_METHOD(bmp);
    DECLARE_STBI_METHOD(png);
    DECLARE_STBI_METHOD(psd);
    DECLARE_STBI_METHOD(gif);
}
