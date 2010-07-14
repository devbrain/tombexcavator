#include "providers/westwood/dune2/dune2_provider.hpp"
#include "providers/westwood/dune2/dune2_fs.hpp"

dune2_c::~dune2_c ()
{
}
// --------------------------------------------------------------
const std::string dune2_c::name () const
{
  return "Dune2";
}
// --------------------------------------------------------------
const std::string dune2_c::description () const
{
  return "Dune2 - 1992 from Westwood";
}
// --------------------------------------------------------------
void dune2_c::version (int& major, int& minor) const
{
  major = 1;
  minor = 0;
}
// --------------------------------------------------------------
vfs::fs_c* dune2_c::get_vfs ()
{
  return new dune2_fs_c;
}
// --------------------------------------------------------------
REGISTER_PROVIDER (dune2_c)
