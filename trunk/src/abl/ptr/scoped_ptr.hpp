#ifndef __UTILS_SCOPED_PTR_HPP__
#define __UTILS_SCOPED_PTR_HPP__

#include "utils/guard/destructor.hpp"
#include "utils/stl/noncopyable.hpp"

namespace utils
{
    template <typename T>
    class scoped_ptr_c : public stl::noncopyable_c
    {
    public:
        scoped_ptr_c (T* ptr);
        ~scoped_ptr_c ();

        T* get ();
        const T* get () const;

        T* operator -> ();
        const T* operator -> () const;

        T* release ();
    private:
        T* m_ptr;
    };
} // ns utils

// ===================================================================
// Implementation
// ===================================================================
namespace utils
{
    template <typename T>
    inline
    scoped_ptr_c<T>::scoped_ptr_c (T* ptr)
        : m_ptr (ptr)
    {
    }
    // --------------------------------------------------------------
    template <typename T>
    inline
    scoped_ptr_c<T>::~scoped_ptr_c ()
    {
        destructor_c <T> d(m_ptr);
        d ();
    }
    // --------------------------------------------------------------
    template <typename T>
    inline
    T* scoped_ptr_c<T>::get ()
    {
        return m_ptr;
    }
    // --------------------------------------------------------------
    template <typename T>
    inline
    const T* scoped_ptr_c<T>::get () const
    {
        return m_ptr;
    }
    // --------------------------------------------------------------
    template <typename T>
    inline
    T* scoped_ptr_c<T>::operator -> ()
    {
        return m_ptr;
    }
    // --------------------------------------------------------------
    template <typename T>
    inline
    const T* scoped_ptr_c<T>::operator -> () const
    {
        return m_ptr;
    }
    // --------------------------------------------------------------
    template <typename T>
    inline
    T* scoped_ptr_c<T>::release ()
    {
        T* v = m_ptr;
        m_ptr = 0;
        return v;
    }
} // ns utils


#endif
