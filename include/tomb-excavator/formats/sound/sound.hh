//
// Created by igor on 28/03/2021.
//

#ifndef TOMBEXCAVATOR_SOUND_HH
#define TOMBEXCAVATOR_SOUND_HH

#include <tomb-excavator/export-formats.h>
#include <cstdint>
#include <vector>
#include <variant>

namespace formats::sound
{
    struct FORMATS_API sound_data
    {
        using u8_sample_t = std::vector<uint8_t>;
        using u16_sample_t = std::vector<uint16_t>;
        using u32_sample_t = std::vector<uint32_t>;
        using float_sample_t = std::vector<float>;



        bool is_mono;
        std::size_t num_channels;
        int bit_depth;
        uint32_t sample_rate;
        std::size_t samples_per_channel;

        std::variant<u8_sample_t , u16_sample_t , u32_sample_t , float_sample_t > samples;

    };


}

#endif //TOMBEXCAVATOR_SOUND_HH
