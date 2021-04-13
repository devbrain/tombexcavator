//
// Created by igor on 13/04/2021.
//
#include <istream>
#include <ostream>
#include <cstring>
#include "westwood_compression.hh"

namespace games::westwood
{
    static inline uint8_t read(std::istream& is)
    {
        uint8_t x;
        is.read((char*) &x, 1);
        return x;
    }
    // ---------------------------------------------------------------------------------------------
    void decompress_format_80(std::vector<uint8_t>& out, std::istream& is)
    {
        auto destLength = out.size();
        uint8_t* dest = out.data();
        uint8_t* start = dest;
        uint8_t* end = dest + destLength;

        while (dest != end)
        {
            uint8_t cmd;
            uint16_t size;
            uint16_t offset;

            cmd = read(is);

            if (cmd == 0x80)
            {
                /* Exit */
                break;

            } else
            {
                if ((cmd & 0x80u) == 0)
                {
                    /* Short move, relative */
                    size = (cmd >> 4u) + 3u;
                    if (size > end - dest)
                    { size = (uint16_t) (end - dest); }

                    offset = ((cmd & 0xFu) << 8u) + read(is);

                    /* This decoder assumes memcpy. As some platforms implement memcpy as memmove, this is much safer */
                    for (; size > 0; size--)
                    {
                        *dest = *(dest - offset);
                        dest++;
                    }

                } else
                {
                    if (cmd == 0xFE)
                    {
                        /* Long set */
                        size = read(is);
                        size += read(is) << 8u;
                        if (size > end - dest)
                        { size = (uint16_t) (end - dest); }
                        auto x = read(is);
                        memset(dest, x, size);
                        dest += size;

                    } else
                    {
                        if (cmd == 0xFF)
                        {
                            /* Long move, absolute */
                            size = read(is);
                            size += read(is) << 8u;
                            if (size > end - dest)
                            { size = (uint16_t) (end - dest); }

                            offset = read(is);
                            offset += read(is) << 8u;

                            /* This decoder assumes memcpy. As some platforms implement memcpy as memmove, this is much safer */
                            for (; size > 0; size--)
                            { *dest++ = start[offset++]; }

                        } else
                        {
                            if ((cmd & 0x40u) != 0)
                            {
                                /* Short move, absolute */
                                size = (cmd & 0x3Fu) + 3;
                                if (size > end - dest)
                                { size = (uint16_t) (end - dest); }

                                offset = read(is);
                                offset += read(is) << 8u;

                                /* This decoder assumes memcpy. As some platforms implement memcpy as memmove, this is much safer */
                                for (; size > 0; size--)
                                { *dest++ = start[offset++]; }

                            } else
                            {
                                /* Short copy */
                                size = cmd & 0x3Fu;
                                if (size > end - dest)
                                { size = (uint16_t) (end - dest); }

                                /* This decoder assumes memcpy. As some platforms implement memcpy as memmove, this is much safer */
                                for (; size > 0; size--)
                                { *dest++ = read(is); }
                            }
                        }
                    }
                }
            }
        }
        auto new_size = (std::size_t) (dest - start);
        if (destLength != new_size)
        {
            out.resize(new_size);
        }
    }
    // ---------------------------------------------------------------------------------------------
    void decompress_format_40(std::vector<uint8_t>& out, std::istream& is)
    {
        uint8_t* dst = out.data();
        uint16_t cmd;
        uint16_t count;

        for (;;)
        {
            cmd = read(is);    /* 8 bit command code */

            if (cmd == 0)
            {
                /* XOR with value */
                auto s = read(is);
                auto v = read(is);
                for (count = s; count > 0; count--)
                {
                    *dst++ ^= v;
                }
            } else
            {
                if ((cmd & 0x80u) == 0)
                {
                    /* XOR with string */
                    for (count = cmd; count > 0; count--)
                    {
                        *dst++ ^= read(is);
                    }
                } else
                {
                    if (cmd != 0x80u)
                    {
                        /* skip bytes */
                        dst += (cmd & 0x7Fu);
                    } else
                    {
                        /* last byte was 0x80 : read 16 bit value */
                        cmd = read(is);
                        cmd += read(is) << 8u;

                        if (cmd == 0)
                        { break; }    /* 0x80 0x00 0x00 => exit code */

                        if ((cmd & 0x8000u) == 0)
                        {
                            /* skip bytes */
                            dst += cmd;
                        } else
                        {
                            if ((cmd & 0x4000u) == 0)
                            {
                                /* XOR with string */
                                for (count = cmd & 0x3FFFu; count > 0; count--)
                                {
                                    *dst++ ^= read(is);
                                }
                            } else
                            {
                                /* XOR with value */
                                auto v = read(is);
                                for (count = cmd & 0x3FFFu; count > 0; count--)
                                {
                                    *dst++ ^= v;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
