#include <iostream>
#include <string>

#include "libvfs/register.hpp"
#include "libvfs/fs.hpp"
#include "libvfs/fs_dir.hpp"
#include "libvfs/fs_file.hpp"
#include "libvfs/dump_visitor.hpp"
#include "libvfs/print_visitor.hpp"
#include "libvfs/for_each.hpp"


#include "abl/dynamic_loader.hpp"
#include "abl/dynamic_library.hpp"
#include "abl/exception.hpp"
#include "abl/fs/path.hpp"

int main (int argc, char* argv [])
{

  if (argc != 3)
    {
      std::cerr << "USAGE: " << argv [0] << " <so_dir> <dune2_dir>" << std::endl;
      return 1;
    }
  try
    {
      std::string so_dir = argv [1];
      std::string pth    = argv [2];
      
      abl::dynamic_loader_c dl (so_dir);
      abl::dynamic_library_c* dune2 = dl.load ("provider_dune2");
      std::cout << "provider loaded" << std::endl;
      
      void* d = dune2->symbol ("internal_vfs_register_fs");
      if (!d)
	{
	  std::cerr << "Failed to load symbol" << std::endl;
	  return 1;
	}
      vfs_register_func_t reg_fn = (vfs_register_func_t) d;
      
      vfs::fs_c* fs = reg_fn ();
      if (!fs->open (pth.c_str ()))
	{
	  std::cerr << "Failed to open DUNE ][" << std::endl;
	  return 1;
	}
      vfs::dump_visitor_c v (abl::path_c ("dune_2"));
      vfs::for_each (fs, &v);
      delete fs;
    }
  catch (abl::exception_c& e)
    {
      std::cerr << "EXCEPTION: " << e.what () << "\t" << e.message () << std::endl;
      return 1;
    }
  return 0;
}
