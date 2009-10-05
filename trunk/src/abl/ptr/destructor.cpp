#include "utils/guard/destructor.hpp"

namespace utils
{
  destructor_c<ACE_Message_Block>::~destructor_c ()
  {
  }
  // -----------------------------------------------------------------------
  void destructor_c<ACE_Message_Block>::operator () ()
  {
    if (_victim ())
      {
	_victim ()->release ();
      }
  }

  
  // ======================================================================

  destructor_c <addrinfo>::~destructor_c ()
  {
  }
  // -----------------------------------------------------------------------
  void destructor_c <addrinfo>::operator () ()
  {
    if (_victim ())
      {
	freeaddrinfo (_victim ());
      }
  }

} // ns utils
