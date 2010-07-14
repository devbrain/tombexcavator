#include <iostream>
#include "libvfs/print_visitor.hpp"
#include "libvfs/fs_object.hpp"
#include "libvfs/fs_file.hpp"
#include "abl/fs/path.hpp"

namespace vfs 
{
  std::string print_visitor_c::describe () const
  {
    return "prints the content of the VFS to the standard output";
  }
  // ----------------------------------------------------------------------------
  bool print_visitor_c::visit (fs_object_c* object, const abl::path_c& path)
  {
    abl::path_c pth = path;
    if (object->type () == fs_object_c::eDIRECTORY)
      {
	pth.make_directory ();
	std::cout << "D. " << pth.to_string () << std::endl;
      }
    else
      {
	pth.make_file ();
	std::string full_path = pth.to_string ();
	fs_file_c* fl = dynamic_cast <fs_file_c*> (object);
	std::size_t fl_size = fl->size ();
	std::cout << "\t" << full_path << " : " << (int)fl_size << std::endl;
      }
    return true;
  }
}
