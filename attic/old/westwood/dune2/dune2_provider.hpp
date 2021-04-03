#ifndef __DUNE2_PROVIDER_HPP__
#define __DUNE2_PROVIDER_HPP__

#include "libprovider/abstract_provider.hpp"

class dune2_c : public provider::abstract_provider_c
{
public:
  virtual ~dune2_c ();
private:
  virtual const std::string name () const;
  virtual const std::string description () const;
  virtual void  version (int& major, int& minor) const;
  virtual vfs::fs_c* get_vfs ();
};


#endif
