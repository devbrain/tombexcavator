//
// Created by igor on 14/02/2021.
//

#include "pcx.hh"
#include <cstring>

#define DR_PCX_IMPLEMENTATION
#include "formats/image/thirdparty/dr_pcx.h"

namespace formats::image
{
    bool is_pcx(const char* input, std::size_t input_length)
    {
        drpcx_memory memory;
        memory.data = (const unsigned char*)input;
        memory.dataSize = input_length;
        memory.currentReadPos = 0;

        drpcx pcx;
        pcx.onRead    = drpcx__on_read_memory;
        pcx.pUserData = &memory;
        pcx.flipped   = DRPCX_FALSE;
        if (pcx.onRead(&memory, &pcx.header, sizeof(pcx.header)) != sizeof(pcx.header))
        {
            return false;    // Failed to read the header.
        }

        if (pcx.header.header != 10)
        {
            return false;    // Not a PCX file.
        }

        if (pcx.header.encoding != 1)
        {
            return false;    // Not supporting non-RLE encoding. Would assume a value of 0 indicates raw, unencoded, but that is apparently never used.
        }

        if (pcx.header.bpp != 1 && pcx.header.bpp != 2 && pcx.header.bpp != 4 && pcx.header.bpp != 8)
        {
            return false;    // Unsupported pixel format.
        }
        return true;
    }

    bool load_pcx(const char* input, std::size_t input_length, picture& out)
    {

        if (!is_pcx(input, input_length))
        {
            return false;
        }

        int width;
        int height;
        int components;
        drpcx_uint8* pImageData = drpcx_load_memory(input, input_length, DRPCX_FALSE, &width, &height, &components, 3);
        if (pImageData == NULL)
        {
            return false;
        }
        out.width = width;
        out.height = height;
        out.bpp = 3;
        out.pixels.resize(out.width*out.height*out.bpp);

        std::memcpy(out.pixels.data(), pImageData, out.pixels.size());
        drpcx_free(pImageData);
        return true;
    }
}