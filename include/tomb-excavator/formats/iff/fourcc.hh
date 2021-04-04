//
// Created by igor on 04/04/2021.
//

#ifndef TOMBEXCAVATOR_FOURCC_HH
#define TOMBEXCAVATOR_FOURCC_HH

#include <cstdint>
#include <iosfwd>

#include <tomb-excavator/export-formats.h>
#include <tomb-excavator/bsw/string_constant.hh>


namespace formats::iff
{
    namespace detail
    {
        inline
        constexpr uint32_t pack_chars(char a, char b, char c, char d)
        {
            return ((uint32_t)a << 24u) | ((uint32_t)b << 16u) | ((uint32_t)c << 8u) | (uint32_t)d;
        }
    }


    class FORMATS_API chunk_type
    {
      friend FORMATS_API bool operator == (const chunk_type& a, const chunk_type& b);
      friend FORMATS_API bool operator != (const chunk_type& a, const chunk_type& b);
      friend FORMATS_API bool operator <  (const chunk_type& a, const chunk_type& b);
      friend FORMATS_API bool operator <= (const chunk_type& a, const chunk_type& b);
      friend FORMATS_API bool operator >  (const chunk_type& a, const chunk_type& b);
      friend FORMATS_API bool operator >= (const chunk_type& a, const chunk_type& b);
    public:
      constexpr explicit chunk_type(const bsw::StringConstant<4>& x)
      : m_id(detail::pack_chars(x[0], x[1], x[2], x[3]))
      {

      }

      explicit chunk_type (uint32_t x)
      : m_id(x) {}

      chunk_type(const chunk_type&) = default;
      chunk_type& operator = (const chunk_type&) = default;

      [[nodiscard]] std::string to_string () const
      {
          auto v = m_id;
          char c1, c2, c3, c4;
          c4 = (char)(v & 0xFFu);
          v >>= 8u;
          c3 = (char)(v & 0xFFu);
          v >>= 8u;
          c2 = (char)(v & 0xFFu);
          v >>= 8u;
          c1 = (char)(v & 0xFFu);
          const char h[] = {c1, c2, c3, c4, 0};
          return std::string(h);
      }

      [[nodiscard]] constexpr uint32_t value() const
      {
          return m_id;
      }
    private:
      uint32_t m_id;
    };

    FORMATS_API std::ostream& operator << (std::ostream& os, const chunk_type& nm);

    template<std::size_t N>
    constexpr chunk_type fourcc(const char(& value)[N])
    {
        return chunk_type{bsw::StringFactory(value)};
    }
}

#endif //TOMBEXCAVATOR_FOURCC_HH
