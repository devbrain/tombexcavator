#ifndef __UTILS_SHARED_PTR_HPP__
#define __UTILS_SHARED_PTR_HPP__

#include <ace/Atomic_Op_T.h>
#include "abl/mt/types.hpp"
#include "utils/guard/destructor.hpp"

namespace utils
{
    class shared_counter_base_c
    {
    public:
        virtual ~shared_counter_base_c ();
        virtual bool release () = 0;
        virtual void inc () = 0;
        virtual unsigned int value () = 0;
    };
    // -------------------------------------------------------------------
    template <class T, class Lock>
    class shared_counter_c : public shared_counter_base_c
    {
    public:
        typedef T    elem_t;
        typedef Lock lock_t;
    public:
        shared_counter_c ();
        shared_counter_c (elem_t* ptr);
        
        ~shared_counter_c ();

        void dispose ();
        virtual bool release ();
        virtual void inc ();
        virtual unsigned int value ();
        
        
    private:
        typedef ACE_Atomic_Op<Lock, unsigned int> counter_t;
        counter_t m_counter;
        elem_t*   m_ptr;
    };
    // ------------------------------------------------------------------------------
    template <class T, class Lock>
    class shared_ptr_c
    {
    public:
        typedef T                        elem_t;
        typedef Lock                     lock_t;
        typedef shared_ptr_c<T,Lock>     self_t;
        typedef shared_counter_c<T,Lock> counter_t;
    public:
        shared_ptr_c ();
        explicit shared_ptr_c (elem_t* ptr);
        shared_ptr_c (const shared_ptr_c& obj);
        shared_ptr_c (elem_t* ptr, shared_counter_base_c* counter);

        ~shared_ptr_c ();
        
        T* get ();
        const T* get () const;

        T* operator -> ();
        const T* operator -> () const;

        self_t* clone ();

        bool release ();
        shared_counter_base_c* counter () const;

        bool null () const;
    private:
        shared_counter_base_c* m_counter;
        T* m_ptr;
    };

    template <class T, class T0, class Lock>
    inline
    shared_ptr_c<T,Lock> shared_cast (shared_ptr_c<T0, Lock> sp)
    {
        return shared_ptr_c<T, Lock> (dynamic_cast<T *>(sp.get ()),
                                      sp.counter ());
    }
} // ns utils
// =========================================================================
// Implementation
// =========================================================================
namespace utils
{
    // =====================================================================
    // shared_counter_c
    // =====================================================================
    template <class T, class Lock>
    inline
    shared_counter_c<T,Lock>::shared_counter_c ()
        : m_counter (1),
          m_ptr (0)
    {
        
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_counter_c<T,Lock>::shared_counter_c (elem_t* ptr)
        : m_counter (1),
          m_ptr (ptr)
    {
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_counter_c<T,Lock>::~shared_counter_c ()
    {
        
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    void shared_counter_c<T,Lock>::dispose ()
    {
        destructor_c<T> d(m_ptr);
        d ();
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    bool shared_counter_c<T,Lock>::release ()
    {
        if (m_ptr && (--m_counter == 0))
        {
            dispose ();
            return true;
        }
        return false;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    void shared_counter_c<T,Lock>::inc ()
    {
        m_counter++;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    unsigned int shared_counter_c<T,Lock>::value ()
    {
        return m_counter.value ();
    }
    // =====================================================================
    // shared_ptr_c
    // =====================================================================
    template <class T, class Lock>
    inline
    shared_ptr_c<T,Lock>::shared_ptr_c ()
        : m_counter (0),
          m_ptr (0)
    {
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_ptr_c<T,Lock>::shared_ptr_c (elem_t* ptr)
        : m_counter (new counter_t (ptr)),
          m_ptr (ptr)
    {
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_ptr_c<T,Lock>::shared_ptr_c (elem_t* ptr, shared_counter_base_c* counter)
        : m_counter (counter),
          m_ptr (0)
    {
        if (ptr)
        {
            m_counter->inc ();
        }
        m_ptr = ptr;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_ptr_c<T,Lock>::shared_ptr_c (const shared_ptr_c& obj)
        : m_counter (obj.counter ()),
          m_ptr (0)
    {
        if (obj.m_ptr)
        {
            m_counter->inc ();
        }
        m_ptr = obj.m_ptr;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_ptr_c<T,Lock>::~shared_ptr_c ()
    {
        this->release ();
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    T* shared_ptr_c<T,Lock>::get ()
    {
        return m_ptr;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    const T* shared_ptr_c<T,Lock>::get () const
    {
        return m_ptr;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    T* shared_ptr_c<T,Lock>::operator -> ()
    {
        return m_ptr;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    const T* shared_ptr_c<T,Lock>::operator -> () const
    {
        return m_ptr;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    typename shared_ptr_c<T,Lock>::self_t* shared_ptr_c<T,Lock>::clone ()
    {
        return new shared_ptr_c<T,Lock>(*this);
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    bool shared_ptr_c<T,Lock>::release ()
    {
        if (!m_counter)
        {
            return false;
        }
        if (m_counter->release ())
        {
            delete m_counter;
            m_counter = 0;
            m_ptr = 0;
            return true;
        }
        return false;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    shared_counter_base_c* shared_ptr_c<T,Lock>::counter () const
    {
        return m_counter;
    }
    // ---------------------------------------------------------------------
    template <class T, class Lock>
    inline
    bool shared_ptr_c<T,Lock>::null () const
    {
        return (m_ptr == 0);
    }
}

#endif
