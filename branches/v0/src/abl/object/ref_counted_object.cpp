#include "abl/object/ref_counted_object.hpp"

namespace abl 
{
  ref_counted_object_c::ref_counted_object_c ()
    : m_counter  (1)
  {
  }
  // ------------------------------------------------------------
  ref_counted_object_c::~ref_counted_object_c ()
  {
  }
  // ------------------------------------------------------------
  int ref_counted_object_c::reference_count () const
  {
    return m_counter.value ();
  }
  // ------------------------------------------------------------
  void ref_counted_object_c::duplicate () const
  {
    ++m_counter;
  }
  // ------------------------------------------------------------
  void ref_counted_object_c::release () const
  {
    if  (--m_counter == 0) delete this;
  }
  // ------------------------------------------------------------
}

