#include <stdio.h>
#include "libvfs/dump_visitor.hpp"
#include "libvfs/fs_object.hpp"
#include "libvfs/fs_file.hpp"

#include "abl/fs/path.hpp"
#include "abl/fs/file.hpp"
#include "abl/buffer.hpp"

namespace vfs  
{
  // ============================================================================
  dump_visitor_c::dump_visitor_c (const abl::path_c& root)
    : extractor_visitor_c (root)
  {
  }
  // ----------------------------------------------------------------------------
  dump_visitor_c::~dump_visitor_c ()
  {
  }
  // ----------------------------------------------------------------------------
  std::string dump_visitor_c::describe () const
  {
    return "dumps the content of the VFS to the file system";
  }
  // ----------------------------------------------------------------------------
  bool dump_visitor_c::_handle_dir  (fs_dir_c* dir, const abl::path_c& path)
  {
    abl::file_c f (path);
    if (!f.exists ())
      {
	f.create_directory ();
      }
    return true;
  }
  // ----------------------------------------------------------------------------
  bool dump_visitor_c::_handle_file (fs_file_c* file, const abl::path_c& path)
  {
    std::string full_path = path.to_string ();
    FILE* f = fopen (full_path.c_str (), "wb");
    std::size_t fl_size = file->size ();
    abl::buffer_c <char> buff (fl_size);
    file->read (buff.begin (), fl_size);
    fwrite (buff.begin (), fl_size, 1, f);
    fclose (f);
    return true;
  }
  
}
