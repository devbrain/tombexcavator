#ifndef MEMARRAY_H_INCLUDED
#define MEMARRAY_H_INCLUDED

#include "types.h"
#include "memarea.h"

#include <assert.h>

template <typename T>
class memarray_c
{
    public:
        memarray_c (memarea_c& mem, types::word offset, types::word len);
        T get (types::word index) const;
        void set (types::word index, T v);
    private:
        memarea_c& m_memory;
        const types::word m_offset;
        const types::word m_length;
};
// --------------------------------------------------------------------------------
template <typename T>
memarray_c<T>::memarray_c (memarea_c& mem, types::word offset, types::word len)
: m_memory (mem),
  m_offset (offset),
  m_length (len)
{
    assert (offset + sizeof (T)*len < 0xFFFF);
}
// --------------------------------------------------------------------------------
template <typename T>
T memarray_c<T>::get (types::word index) const
{
    assert (index < m_length);
    T v;
    m_memory.get (m_offset + sizeof (T)*index, v);
    return v;
}
// --------------------------------------------------------------------------------
template <typename T>
void memarray_c<T>::set (types::word index, T v)
{
    assert (index < m_length);
    m_memory.set (m_offset + sizeof (T)*index, v);
}

#endif // MEMARRAY_H_INCLUDED
