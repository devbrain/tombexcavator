#ifndef __ABL_PTR_SHARED_PTR_HPP__
#define __ABL_PTR_SHARED_PTR_HPP__


#include "abl/exception.hpp"
#include "abl/mt/atomic_counter.hpp"
#include "abl/mt/mutex.hpp"
#include <algorithm>
#include <assert.h>

namespace abl 
{

  /// Simple ReferenceCounter object, does not delete itself when count reaches 0.
  class reference_counter_c
  {
  public:
    reference_counter_c(): _cnt(1)
    {
    }
  
    void duplicate()
    {
      ++_cnt;
    }
  
    int release()
    {
      return --_cnt;
    }
  
    int referenceCount() const
    {
      return _cnt.value();
    }
  
  private:
    atomic_counter_c <mutex_c, int> _cnt;
  };


  /// The default release policy for SharedPtr, which
  /// simply uses the delete operator to delete an object.
  template <class C>
  class release_policy_c
  {
  public:
    static void release(C* pObj)
    /// Delete the object.
    /// Note that pObj can be 0.
    {
      delete pObj;
    }
  };


  /// The release policy for SharedPtr holding arrays.
  template <class C>
  class release_array_policy
	
  {
  public:
    static void release(C* pObj)
    /// Delete the object.
    /// Note that pObj can be 0.
    {
      delete [] pObj;
    }
  };

  /// shared_ptr_c is a "smart" pointer for classes implementing
  /// reference counting based garbage collection.
  /// shared_ptr_c is thus similar to AutoPtr. Unlike the
  /// AutoPtr template, which can only be used with
  /// classes that support reference counting, shared_ptr_c
  /// can be used with any class. For this to work, a
  /// shared_ptr_c manages a reference count for the object
  /// it manages.
  /// 
  /// shared_ptr_c works in the following way:
  /// If an shared_ptr_c is assigned an ordinary pointer to
  /// an object (via the constructor or the assignment operator),
  /// it takes ownership of the object and the object's reference 
  /// count is initialized to one.
  /// If the shared_ptr_c is assigned another shared_ptr_c, the
  /// object's reference count is incremented by one.
  /// The destructor of shared_ptr_c decrements the object's
  /// reference count by one and deletes the object if the
  /// reference count reaches zero.
  /// shared_ptr_c supports dereferencing with both the ->
  /// and the * operator. An attempt to dereference a null
  /// shared_ptr_c results in a NullPointerException being thrown.
  /// shared_ptr_c also implements all relational operators and
  /// a cast operator in case dynamic casting of the encapsulated data types
  /// is required.
  template <class C, class RC = reference_counter_c, class RP = release_policy_c<C> >
  class shared_ptr_c
	
  {
  public:
    shared_ptr_c(): _pCounter(new RC), _ptr(0)
    {
    }

    shared_ptr_c(C* ptr): _pCounter(new RC), _ptr(ptr)
    {
    }

    template <class Other, class OtherRP> 
    shared_ptr_c(const shared_ptr_c<Other, RC, OtherRP>& ptr): _pCounter(ptr._pCounter), _ptr(const_cast<Other*>(ptr.get()))
    {
      _pCounter->duplicate();
    }

    shared_ptr_c(const shared_ptr_c& ptr): _pCounter(ptr._pCounter), _ptr(ptr._ptr)
    {
      _pCounter->duplicate();
    }

    ~shared_ptr_c()
    {
      release();
    }

    shared_ptr_c& assign(C* ptr)
    {
      if (get() != ptr)
	{
	  RC* pTmp = new RC;
	  release();
	  _pCounter = pTmp;
	  _ptr = ptr;
	}
      return *this;
    }
	
    shared_ptr_c& assign(const shared_ptr_c& ptr)
    {
      if (&ptr != this)
	{
	  shared_ptr_c tmp(ptr);
	  swap(tmp);
	}
      return *this;
    }
	
    template <class Other, class OtherRP>
    shared_ptr_c& assign(const shared_ptr_c<Other, RC, OtherRP>& ptr)
    {
      if (ptr.get() != _ptr)
	{
	  shared_ptr_c tmp(ptr);
	  swap(tmp);
	}
      return *this;
    }

    shared_ptr_c& operator = (C* ptr)
    {
      return assign(ptr);
    }

    shared_ptr_c& operator = (const shared_ptr_c& ptr)
    {
      return assign(ptr);
    }

    template <class Other, class OtherRP>
    shared_ptr_c& operator = (const shared_ptr_c<Other, RC, OtherRP>& ptr)
    {
      return assign<Other>(ptr);
    }

    void swap(shared_ptr_c& ptr)
    {
      std::swap(_ptr, ptr._ptr);
      std::swap(_pCounter, ptr._pCounter);
    }

    template <class Other> 
    shared_ptr_c<Other, RC, RP> cast() const
    /// Casts the shared_ptr_c via a dynamic cast to the given type.
    /// Returns an shared_ptr_c containing NULL if the cast fails.
    /// Example: (assume class Sub: public Super)
    ///    shared_ptr_c<Super> super(new Sub());
    ///    shared_ptr_c<Sub> sub = super.cast<Sub>();
    ///    poco_assert (sub.get());
    {
      Other* pOther = dynamic_cast<Other*>(_ptr);
      if (pOther)
	return shared_ptr_c<Other, RC, RP>(_pCounter, pOther);
      return shared_ptr_c<Other, RC, RP>();
    }

    template <class Other> 
    shared_ptr_c<Other, RC, RP> unsafeCast() const
    /// Casts the shared_ptr_c via a static cast to the given type.
    /// Example: (assume class Sub: public Super)
    ///    shared_ptr_c<Super> super(new Sub());
    ///    shared_ptr_c<Sub> sub = super.unsafeCast<Sub>();
    ///    poco_assert (sub.get());
    {
      Other* pOther = static_cast<Other*>(_ptr);
      return shared_ptr_c<Other, RC, RP>(_pCounter, pOther);
    }

    C* operator -> ()
    {
      return deref();
    }

    const C* operator -> () const
    {
      return deref();
    }

    C& operator * ()
    {
      return *deref();
    }

    const C& operator * () const
    {
      return *deref();
    }

    C* get()
    {
      return _ptr;
    }

    const C* get() const
    {
      return _ptr;
    }

    operator C* ()
    {
      return _ptr;
    }
	
    operator const C* () const
    {
      return _ptr;
    }

    bool operator ! () const
    {
      return _ptr == 0;
    }

    bool isNull() const
    {
      return _ptr == 0;
    }

    bool operator == (const shared_ptr_c& ptr) const
    {
      return get() == ptr.get();
    }

    bool operator == (const C* ptr) const
    {
      return get() == ptr;
    }

    bool operator == (C* ptr) const
    {
      return get() == ptr;
    }

    bool operator != (const shared_ptr_c& ptr) const
    {
      return get() != ptr.get();
    }

    bool operator != (const C* ptr) const
    {
      return get() != ptr;
    }

    bool operator != (C* ptr) const
    {
      return get() != ptr;
    }

    bool operator < (const shared_ptr_c& ptr) const
    {
      return get() < ptr.get();
    }

    bool operator < (const C* ptr) const
    {
      return get() < ptr;
    }

    bool operator < (C* ptr) const
    {
      return get() < ptr;
    }

    bool operator <= (const shared_ptr_c& ptr) const
    {
      return get() <= ptr.get();
    }

    bool operator <= (const C* ptr) const
    {
      return get() <= ptr;
    }

    bool operator <= (C* ptr) const
    {
      return get() <= ptr;
    }

    bool operator > (const shared_ptr_c& ptr) const
    {
      return get() > ptr.get();
    }

    bool operator > (const C* ptr) const
    {
      return get() > ptr;
    }

    bool operator > (C* ptr) const
    {
      return get() > ptr;
    }

    bool operator >= (const shared_ptr_c& ptr) const
    {
      return get() >= ptr.get();
    }

    bool operator >= (const C* ptr) const
    {
      return get() >= ptr;
    }

    bool operator >= (C* ptr) const
    {
      return get() >= ptr;
    }
	
    int referenceCount() const
    {
      return _pCounter->referenceCount();
    }

  private:
    C* deref() const
    {
      if (!_ptr)
	throw null_pointer_exception_c ();

      return _ptr;
    }

    void release()
    {
      assert (_pCounter);
      int i = _pCounter->release();
      if (i == 0)
	{
	  RP::release(_ptr);
	  _ptr = 0;

	  delete _pCounter;
	  _pCounter = 0;
	}
    }

    shared_ptr_c(RC* pCounter, C* ptr): _pCounter(pCounter), _ptr(ptr)
		/// for cast operation
    {
      assert (_pCounter);
      _pCounter->duplicate();
    }

  private:
    RC* _pCounter;
    C*  _ptr;

    template <class OtherC, class OtherRC, class OtherRP> friend class shared_ptr_c;
  };


  template <class C, class RC, class RP>
  inline void swap(shared_ptr_c<C, RC, RP>& p1, shared_ptr_c<C, RC, RP>& p2)
  {
    p1.swap(p2);
  }


} 


#endif 
