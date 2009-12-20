#include <iostream>

#include "dos_loader.hpp"

int main (int argc, char* argv[])
{
	dos_exe_c de = dos_exe_c::load (argv[1]);
	std::cout << "DOS HEADER OF " << argv [1] << std::endl << de << std::endl;
	return 0;
}
