#include <iostream>
#include <set>
#include <string>

#include "abl/fs/glob.hpp"

int main (int argc, char* argv [])
{

  if (argc != 2)
    {
      std::cerr << "USAGE: " << argv [0] << " <pattern>" << std::endl;
      return 1;
    }
  const std::string pattern = argv [1];
  typedef std::set <std::string> files_t;
  files_t files;

  const int options = abl::glob_c::GLOB_CASELESS;
  abl::glob_c::glob (pattern, files, options);
  for (files_t::const_iterator i = files.begin (); i != files.end (); i++)
    {
      std::cout << *i << std::endl;
    }
  return 0;
}
