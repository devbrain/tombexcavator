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

#include "thirdparty/tclap/CmdLine.h"
#include "thirdparty/predef/predef.h"

int main (int argc, char* argv [])
{

  // parse command line ===========================
  using namespace TCLAP;

  CmdLine cmd ("extractor", ' ', "1.0");
  ValueArg <std::string> so_path ("d" , "dso_path", "DSO path", 
				  false, ""    , "path"           , cmd);
  ValueArg <std::string> provider ("p" , "provider", "provider name", 
				   true, ""       , "name"        , cmd);
  ValueArg <std::string> input_path ("i" , "input", "input path", 
				     true, ""       , "name"        , cmd);

  ValueArg <std::string> output_path ("o" , "output", "output path", 
				      false, "", "path"        , cmd);

  SwitchArg list_arg ("l", "list", "list", cmd, false);
  cmd.parse(argc, argv);
  
  if (!list_arg.getValue ())
    {
      if (!output_path.isSet ())
	{
	  std::cerr << "use -o to specify output dir" << std::endl;
	  return 1;
	}
    }


  try
    {
      std::string so_dir;
      if (so_path.isSet ())
	{
	  so_dir = so_path.getValue ();
	}
      else
	{
#if !defined(NDEBUG)
	  std::string prefix;
#if defined(PREDEF_OS_WINDOWS)
	  std::string ld ("bin");	
	  prefix = "Windows";
#elif defined(PREDEF_OS_LINUX)
	  std::string ld ("lib");
	  prefix = "Linux";
#endif
#endif // NDEBUG
	  if (!prefix.empty ())
	    {
	      abl::path_c tmp_path (ld);
	      tmp_path.make_directory ();
	      tmp_path.push_directory (prefix + ".Debug");
	      so_dir = tmp_path.to_string ();
	    }
	}
      if (so_dir.empty ())
	{
	  std::cerr << "Can not determine providers directory. Use -d flag" << std::endl;
	  return 1;
	}
      std::string pth    = input_path.getValue ();
      std::string prov   = provider.getValue ();
      std::string prov_n = std::string ("provider_") + prov;
      
      std::cout << "Loading provider " << prov_n << " from " << so_dir << std::endl; 

      abl::dynamic_loader_c dl (so_dir);
      abl::dynamic_library_c* so_prov = dl.load (prov_n);
      if (!so_prov) 
	{
	  std::cerr << "Failed to load provider: " << prov_n << " from " << so_dir << std::endl;
	}
      
      void* d = so_prov->symbol ("internal_vfs_register_fs");
      if (!d)
	{
	  std::cerr << "Failed to load symbol" << std::endl;
	  return 1;
	}
      vfs_register_func_t reg_fn = (vfs_register_func_t) d;
      
      vfs::fs_c* fs = reg_fn ();
      if (!fs->open (pth.c_str ()))
	{
	  std::cerr << "Failed to open input " << pth << std::endl;
	  return 1;
	}
      vfs::visitor_c* v;
      if (list_arg.getValue ())
	{
	  v = new vfs::print_visitor_c;
	}
      else
	{
	  v = new vfs::dump_visitor_c (output_path.getValue ());
	}
      if (!vfs::for_each (fs, v))
	{
	  std::cerr << "==== FOR EACH FAILED === " << std::endl;
	}
      std::cout << " ---------------------- " << std::endl
		<< "Finished" << std::endl;
      delete v;
      delete fs;
    }
  catch (abl::exception_c& e)
    {
      std::cerr << "EXCEPTION: " << e.what () << "\t" << e.message () << std::endl;
      return 1;
    }
  catch (std::exception& e)
    {
      std::cerr << "EXCEPTION: " << e.what () << std::endl;
      return 1;
    }
  return 0;
}
