#include "libvfs/fs_object.hpp"

namespace vfs
{
  fs_object_c::fs_object_c (fs_c* owner)
    : m_owner (owner)
  {
  }
  // ---------------------------------------------------------
  fs_object_c::~fs_object_c ()
  {
  }
  // ---------------------------------------------------------
  fs_c* fs_object_c::owner ()
  {
    return m_owner;
  }
  // ---------------------------------------------------------
  const fs_c* fs_object_c::owner () const
  {
    return m_owner;
  }
}


