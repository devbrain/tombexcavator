//
// Created by igor on 07/02/2021.
//

#include "io_base.hh"

namespace formats
{
    io_base::io_base()
    : m_is_big_endian(false)
    {

    }
    // ------------------------------------------------------------------------------------
    bool io_base::big_endian() const
    {
        return m_is_big_endian;
    }
    // ------------------------------------------------------------------------------------
    void io_base::big_endian(bool v)
    {
        m_is_big_endian = v;
    }

    // =============================================================================

    io_base& operator >> (io_base& inp, detail::big_endian)
    {
        inp.big_endian(true);
        return inp;
    }
    // ------------------------------------------------------------------------------------
    io_base& operator >> (io_base& inp, detail::little_endian)
    {
        inp.big_endian(false);
        return inp;
    }
    // ------------------------------------------------------------------------------------
    io_base& operator << (io_base& inp, detail::big_endian)
    {
        inp.big_endian(true);
        return inp;
    }
    // ------------------------------------------------------------------------------------
    io_base& operator << (io_base& inp, detail::little_endian)
    {
        inp.big_endian(false);
        return inp;
    }
} // ns formats
