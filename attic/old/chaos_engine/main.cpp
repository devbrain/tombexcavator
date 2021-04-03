#include <iostream>
#include "archive.h"
#include "memarea.h"
// ===========================================================================
const char* chaos = "c:\\dos\\chaos\\chaos.dat";
// ===========================================================================
int main (int argc, char* argv [])
{
    try
    {
        chaos_archive_c arc (chaos);
#if  0
        std::cout << "Entries number: " << arc.size () << std::endl;
        std::cout.setf ( std::ios_base::left);
        for (unsigned int id = 0; id < arc.size (); id++)
        {
            off_t              size = arc.entry_size (id);
            const std::string& name = arc.entry_name (id);

            std::cout << std::setw (3)  << id << ") "
                      << std::setw (12) << name << " "
                      << std::setw (6)  << size
                      << "   0x" << std::hex << arc.entry_offset (id)
                      << std::dec
                      << std::endl;
        }
#endif
        arc.simul ();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << "ERROR: " << e.what () << std::endl;
    }
    return 0;
}


