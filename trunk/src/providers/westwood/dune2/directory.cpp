#include "providers/westwood/dune2/directory.hpp"
#include "providers/westwood/libwestwood/pak_file.hpp"

struct directory_impl_s
{
  westwood::pak_file_c* m_pak_file;
  vfs::inode_num_t      m_my_inode;
};

directory_c::directory_c (vfs::inode_num_t my_inode);
directory_c::~directory_c ();
  
bool directory_c::open (const char* path);

vfs::inode_num_t    directory_c::inode_num () const;
vfs::inode_num_t    directory_c::parent_inode_num () const;
std::size_t         directory_c::size      () const;
std::size_t directory_c::read (std::size_t entry_num, std::string& name, inode_num_t& inode_num);

