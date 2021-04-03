#ifndef MEMAREA_H_INCLUDED
#define MEMAREA_H_INCLUDED

#include "types.h"

class memarea_c
{
    public:
        memarea_c ();

        memarea_c (const char* dumpfile);

        void dump (const char* dumpfile, types::word seg, types::word offset, types::word len);

        types::word low_mark  () const;
        types::word high_mark () const;
        types::word area_size () const;

        void set_byte  (types::word offset, types::byte b);
        void set_word  (types::word offset, types::word b);
        void set_dword (types::word offset, types::dword b);


        void fill_byte  (types::word offset, types::word count, types::byte b);
        void fill_word  (types::word offset, types::word count, types::word b);
        void fill_dword (types::word offset, types::word count, types::dword b);

        types::byte  get_byte  (types::word offset) const;
        types::word  get_word  (types::word offset) const;
        types::dword get_dword (types::word offset) const;

        template <typename T>
        void set (types::word offset, T v);

        template <typename T>
        void get (types::word offset, T& v) const;


        template <typename T>
        void fill (types::word offset, types::word count, T v);

    private:
        void _update_access (types::word offset) const;
    private:
        types::byte m_area [0xFFFF];

        mutable types::word m_low;
        mutable types::word m_high;
};
// =====================================================================================
template <typename T>
struct memarea_setter_s;

#define DECLARE_MEMAREA_SETTER(TYPE,F)\
template <>\
struct memarea_setter_s<TYPE> { void call (memarea_c& m, types::word ofs, TYPE v) const { m.F (ofs, v); } }

DECLARE_MEMAREA_SETTER(types::byte , set_byte);
DECLARE_MEMAREA_SETTER(types::word , set_word);
DECLARE_MEMAREA_SETTER(types::dword, set_dword);

template <typename T>
struct memarea_getter_s;

#define DECLARE_MEMAREA_GETTER(TYPE,F)\
template <>\
struct memarea_getter_s<TYPE> { void call (const memarea_c& m, types::word ofs, TYPE &v) const { v = m.F (ofs); } }

DECLARE_MEMAREA_GETTER(types::byte , get_byte);
DECLARE_MEMAREA_GETTER(types::word , get_word);
DECLARE_MEMAREA_GETTER(types::dword, get_dword);

template <typename T>
struct memarea_filler_s;

#define DECLARE_MEMAREA_FILLER(TYPE,F)\
template <>\
struct memarea_filler_s<TYPE> { void call (memarea_c& m, types::word ofs, types::word c, TYPE v) const { m.F (ofs, c, v); } }

DECLARE_MEMAREA_FILLER(types::byte , fill_byte);
DECLARE_MEMAREA_FILLER(types::word , fill_word);
DECLARE_MEMAREA_FILLER(types::dword, fill_dword);

// ============================================================================================
template <typename T>
void memarea_c::set (types::word offset, T v)
{
    static memarea_setter_s <T> setter;
    setter.call (*this, offset, v);
}
// ---------------------------------------------------------------------------------------------
template <typename T>
void memarea_c::get (types::word offset, T& v) const
{
    static memarea_getter_s <T> getter;
    getter.call (*this, offset, v);
}
// ---------------------------------------------------------------------------------------------
template <typename T>
void memarea_c::fill (types::word offset, types::word count, T v)
{
    static memarea_filler_s <T> filler;
    filler.call (*this, offset, count, v);
}

#endif // MEMAREA_H_INCLUDED
