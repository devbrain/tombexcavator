//
// Created by igor on 27/05/2020.
//

#include "tomb-excavator/provider/dto/video//palette.hh"
#include <tomb-excavator/bsw/exceptions.hh>

namespace provider::dto
{
    rgba::rgba()
    : r(0), g(0), b(0), a(0xFF)
    {

    }
    // -------------------------------------------------------------------------------------------------------
    rgba::rgba(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
    : r(r_), g(g_), b(b_), a(a_)
    {

    }
    // -------------------------------------------------------------------------------------------------------
    palette::palette(const unsigned char* data, std::size_t size, bool do_correct)
    {
        init (data, size, do_correct);
    }
    // -------------------------------------------------------------------------------------------------------
    palette::palette(const std::vector<unsigned char>& data, bool do_correct)
    {
        init (data.data(), data.size(), do_correct);
    }
    // -------------------------------------------------------------------------------------------------------
    bool palette::empty() const noexcept
    {
        return m_data.empty();
    }
    // -------------------------------------------------------------------------------------------------------
    std::size_t palette::size () const noexcept
    {
        return m_data.size();
    }
    // -------------------------------------------------------------------------------------------------------
    rgba palette::operator [] (std::size_t idx) const
    {
        return m_data [idx];
    }
    // -------------------------------------------------------------------------------------------------------
    void palette::init(const unsigned char* data, std::size_t size, bool do_correct)
    {
        if (size % 3 == 0)
        {
            m_data.resize(size/3);
            for (std::size_t i=0; i<m_data.size(); i++)
            {
                m_data[i] = rgba(data[3*i], data[3*i+1], data[3*i+2]);
            }
        }
        else
        {
            if (size % 4 == 0)
            {
                m_data.resize(size/4);
                for (std::size_t i=0; i<m_data.size(); i++)
                {
                    m_data[i] = rgba(data[4*i], data[4*i+1], data[4*i+2], data[4*i+3]);
                }
            }
            else
            {
                RAISE_EX ("Palette size should be divisible by 3 or 4");
            }
        }
        if (do_correct)
        {
            for (std::size_t i=0; i<m_data.size(); i++)
            {
                auto& color = m_data[i];
                color.r = color.r*4;
                color.g = color.g*4;
                color.b = color.b*4;
                if (color.a != 0xFF)
                {
                    color.a = color.a*4;
                }
            }
        }
    }
}
