#ifndef __UTILS_GUARD_DESTRUCTOR_HPP__
#define __UTILS_GUARD_DESTRUCTOR_HPP__

#include <ace/Message_Block.h>


#ifndef WIN32
   #include <unistd.h>
   #include <netdb.h>
#else
  #if defined(__MINGW32__)
  #if defined(_WIN32_WINNT)
      #undef _WIN32_WINNT
  #endif
  #define _WIN32_WINNT 0x501
  #include <windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <winsock2.h>
  #include <ws2tcpip.h>
  #include <wspiapi.h>
#endif	
   
#endif

namespace utils
{
  template <class T>
  class destructor_c
  {
  public:
    typedef T elem_t;
  public:
    destructor_c (elem_t* victim);
    virtual ~destructor_c ();
    virtual void operator () ();
  protected:
    elem_t* _victim ();
  private:
    elem_t* m_victim;
  };
  // -------------------------------------------------------------------
  template <>
  class destructor_c <ACE_Message_Block>
  {
  public:
    typedef ACE_Message_Block elem_t;
  public:
    destructor_c (elem_t* victim);
    virtual ~destructor_c ();
    virtual void operator () ();
  protected:
    elem_t* _victim ();
  private:
    elem_t* m_victim;
  };
  // -------------------------------------------------------------------
  
  // -------------------------------------------------------------------
  template <>
  class destructor_c <addrinfo>
  {
  public:
    typedef addrinfo elem_t;
  public:
    destructor_c (elem_t* victim);
    virtual ~destructor_c ();
    virtual void operator () ();
  protected:
    elem_t* _victim ();
  private:
    elem_t* m_victim;
  };
  // -------------------------------------------------------------------
  template <class T>
  class array_destructor_c : public destructor_c <T>
  {
    typedef typename destructor_c <T>::elem_t elem_t;
  public:
    array_destructor_c (elem_t* victim);
    virtual void operator () ();
  };
  // -------------------------------------------------------------------
} // ns utils

// ===========================================================================
// Implementation
// ===========================================================================
namespace utils
{
  template <class T>
  inline
  destructor_c<T>::destructor_c (elem_t* victim)
    : m_victim (victim)
  {
  }
  // -----------------------------------------------------------------------
  template <class T>
  destructor_c<T>::~destructor_c ()
  {
  }
  // -----------------------------------------------------------------------
  template <class T>
  void destructor_c<T>::operator () ()
  {
    if (_victim ())
      {
	delete _victim ();
      }
  }
  // -----------------------------------------------------------------------
  template <class T>
  inline
  typename destructor_c<T>::elem_t* destructor_c<T>::_victim ()
  {
    return m_victim;
  }
  // =======================================================================
  inline
  destructor_c<ACE_Message_Block>::destructor_c (elem_t* victim)
    : m_victim (victim)
  {
  }
  // -----------------------------------------------------------------------
  inline
  destructor_c<ACE_Message_Block>::elem_t* destructor_c<ACE_Message_Block>::_victim ()
  {
    return m_victim;
  }
  // =======================================================================
  inline
  destructor_c <addrinfo>::destructor_c (elem_t* victim)
    : m_victim (victim)
  {
  }
  // -----------------------------------------------------------------------
  inline
  destructor_c <addrinfo>::elem_t* destructor_c <addrinfo>::_victim ()
  {
    return m_victim;
  }
  // =======================================================================


  template <class T>
  inline
  array_destructor_c<T>::array_destructor_c (elem_t* victim)
    : destructor_c<T>(victim)
  {
  }
  // -----------------------------------------------------------------------
  template <class T>
  void array_destructor_c<T>::operator () ()
  {
    if (this->_victim ())
      {
	delete [] this->_victim ();
      }
  }
}


#endif
