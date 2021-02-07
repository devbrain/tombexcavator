//
// Created by igor on 07/02/2021.
//

#ifndef TOMBEXCAVATOR_IO_HH
#define TOMBEXCAVATOR_IO_HH

#include "formats/io/input.hh"
#include "formats/io/output.hh"

namespace formats
{
    namespace detail
    {
        enum class endianity
        {
            BIG,
            LITTLE
        };

        template<typename T, endianity e>
        struct endian_data
        {
            static constexpr bool is_big_endian = e == endianity::BIG;
            using value_type = T;

            endian_data()
            : data(0) {}

            endian_data(T x)
            :data (x) {}

            T data;

            operator T() const noexcept { return data; }
        };
    } // ns detail

    using be_uint16_t = detail::endian_data<uint16_t, detail::endianity::BIG>;
    using be_uint32_t = detail::endian_data<uint32_t, detail::endianity::BIG>;
    using be_uint64_t = detail::endian_data<uint64_t, detail::endianity::BIG>;
    using be_int16_t = detail::endian_data<int16_t, detail::endianity::BIG>;
    using be_int32_t = detail::endian_data<int32_t, detail::endianity::BIG>;
    using be_int64_t = detail::endian_data<int64_t, detail::endianity::BIG>;

    using le_uint16_t = detail::endian_data<uint16_t, detail::endianity::LITTLE>;
    using le_uint32_t = detail::endian_data<uint32_t, detail::endianity::LITTLE>;
    using le_uint64_t = detail::endian_data<uint64_t, detail::endianity::LITTLE>;
    using le_int16_t = detail::endian_data<int16_t, detail::endianity::LITTLE>;
    using le_int32_t = detail::endian_data<int32_t, detail::endianity::LITTLE>;
    using le_int64_t = detail::endian_data<int64_t, detail::endianity::LITTLE>;

    template <typename T, detail::endianity E>
    input& operator >> (input& inp, detail::endian_data<T,E>& val)
    {
        const bool current_endianity = inp.big_endian();
        if constexpr (E == detail::endianity::BIG)
        {
            inp.big_endian(true);
        }
        else
        {
            inp.big_endian(false);
        }
        inp.read(val.data);
        inp.big_endian(current_endianity);
        return inp;
    }

    template <typename T, detail::endianity E>
    output& operator << (output& out, detail::endian_data<T,E>& val)
    {
        const bool current_endianity = out.big_endian();
        if constexpr (E == detail::endianity::BIG)
        {
            out.big_endian(true);
        }
        else
        {
            out.big_endian(false);
        }
        out.write(val.data);
        out.big_endian(current_endianity);
        return out;
    }
} // ns formats

#endif //TOMBEXCAVATOR_IO_HH
