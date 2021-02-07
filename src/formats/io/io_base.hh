//
// Created by igor on 07/02/2021.
//

#ifndef TOMBEXCAVATOR_IO_BASE_HH
#define TOMBEXCAVATOR_IO_BASE_HH

#include <tombexcavator-formats_export.h>
#include <endian.hpp>
#include <mio/mmap.hpp>
#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <filesystem>
#include <vector>

namespace formats
{
    class FORMATS_API io_base
    {
    public:
        io_base();
        io_base(const io_base&) = delete;
        io_base& operator = (const io_base&) = delete;

        ~io_base() = default;

        [[nodiscard]] virtual std::size_t tell () const = 0;

        virtual void seek (std::size_t offset) = 0;
        [[nodiscard]] bool big_endian() const;
        void big_endian(bool v);
    private:
        bool m_is_big_endian;
    };

    namespace detail
    {
        struct big_endian
        {
        };
        struct little_endian
        {
        };
    } // ns detail

    inline constexpr detail::big_endian big_endian;
    inline constexpr detail::little_endian little_endian;

    FORMATS_API io_base& operator >> (io_base& inp, detail::big_endian);
    FORMATS_API io_base& operator >> (io_base& inp, detail::little_endian);
    FORMATS_API io_base& operator << (io_base& inp, detail::big_endian);
    FORMATS_API io_base& operator << (io_base& inp, detail::little_endian);
} // ns formats

#endif //TOMBEXCAVATOR_IO_BASE_HH
