//
// Created by igor on 27/05/2020.
//

#ifndef ARCHIVE_PALETTE_HH
#define ARCHIVE_PALETTE_HH

#include <archive/archive_api.h>
#include <vector>
#include <cstdint>

namespace archive
{
    struct ARCHIVE_API rgba
    {
        rgba();
        rgba(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_=0xFF);

        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

    class ARCHIVE_API palette
    {
    public:
        palette() = default;
        palette(const unsigned char* data, std::size_t size, bool do_correct = true);
        palette(const std::vector<unsigned char>& data, bool do_correct = true);
        [[nodiscard]] bool empty() const noexcept ;
        [[nodiscard]] std::size_t size () const noexcept ;
        [[nodiscard]] rgba operator [] (std::size_t idx) const;
    private:
        void init(const unsigned char* data, std::size_t size, bool do_correct);
        std::vector<rgba> m_data;
    };
} // ns archive

#endif //TOMBEXCAVATOR_PALETTE_HH
