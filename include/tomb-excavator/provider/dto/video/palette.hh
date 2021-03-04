//
// Created by igor on 27/05/2020.
//

#ifndef COMMON_PALETTE_HH
#define COMMON_PALETTE_HH

#include <tomb-excavator/export-provider.h>
#include <vector>
#include <cstdint>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace provider::dto
{
    struct PROVIDER_API rgba
    {
        rgba();
        rgba(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_=0xFF);

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    class PROVIDER_API palette
    {
    public:
        palette() = default;
        palette(const unsigned char* data, std::size_t size, bool do_correct = true);
        palette(const std::vector<unsigned char>& data, bool do_correct = true);
        [[nodiscard]] bool empty() const noexcept ;
        [[nodiscard]] std::size_t size () const noexcept ;
        [[nodiscard]] rgba operator [] (std::size_t idx) const;
    protected:
        void init(const unsigned char* data, std::size_t size, bool do_correct);
        std::vector<rgba> m_data;
    };
} // ns
#include <tomb-excavator/msvc/c4251-end.h>
#endif
