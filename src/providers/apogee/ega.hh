//
// Created by igor on 19/02/2021.
//

#ifndef TOMBEXCAVATOR_EGA_HH
#define TOMBEXCAVATOR_EGA_HH

#include <cstdint>
#include <array>
#include <vector>
#include <tomb-excavator/bsw/exceptions.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>

namespace apogee
{
    namespace detail
    {
        template<bool MSB_LEFTMOST>
        struct mask_traits
        {
            static constexpr std::array<uint8_t, 8> masks{
                    (uint8_t) (1 << 0),
                    (uint8_t) (1 << 1),
                    (uint8_t) (1 << 2),
                    (uint8_t) (1 << 3),
                    (uint8_t) (1 << 4),
                    (uint8_t) (1 << 5),
                    (uint8_t) (1 << 6),
                    (uint8_t) (1 << 7),
            };
        };

        template<>
        struct mask_traits<false>
        {
            static constexpr std::array<uint8_t, 8> masks{
                    (uint8_t) (1 << 7),
                    (uint8_t) (1 << 6),
                    (uint8_t) (1 << 5),
                    (uint8_t) (1 << 4),
                    (uint8_t) (1 << 3),
                    (uint8_t) (1 << 2),
                    (uint8_t) (1 << 1),
                    (uint8_t) (1 << 0),
            };
        };

        template<bool MSB_LEFTMOST, int RED_POS, int BLUE_POS, int GREEN_POS, int INTENS_POS, int MASK_POS>
        class ega_byte_planar
        {
        public:
            static constexpr std::array<uint8_t, 4> color_mask = {
                    (uint8_t)(1 << 0),
                    (uint8_t)(1 << 1),
                    (uint8_t)(1 << 2),
                    (uint8_t)(1 << 3)
            };
            static constexpr std::array<int, 4> positions{BLUE_POS, GREEN_POS, RED_POS, INTENS_POS};
        public:
            ega_byte_planar(bsw::io::binary_reader& reader)
                    : m_reader(reader)
            {
            }

            void read_chunk(std::array<uint8_t, 8>& pixels, std::array<uint8_t, 8>& mask)
            {
                pixels.fill(0);
                mask.fill(0);
                uint8_t data [5];
                m_reader.read_raw(data, sizeof(data));
                for (int i = 0; i < 4; i++)
                {
                    const uint8_t val = data[positions[i]];
                    for (unsigned j = 0; j < 8; j++)
                    {
                        if (val & detail::mask_traits<MSB_LEFTMOST>::masks[j])
                        {
                            pixels[j] |= color_mask[i];
                        }
                    }
                }
                for (unsigned j = 0; j < 8; j++)
                {
                    const uint8_t val = data[MASK_POS];
                    if (val & detail::mask_traits<MSB_LEFTMOST>::masks[j])
                    {
                        mask[j] = 1;
                    }
                }
            }
        private:
            bsw::io::binary_reader& m_reader;
        };
    } // ns detail
}

#endif //TOMBEXCAVATOR_EGA_HH
