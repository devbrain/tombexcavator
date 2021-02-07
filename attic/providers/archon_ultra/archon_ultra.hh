//
// Created by igor on 26/05/2020.
//

#ifndef TOMBEXCAVATOR_ARCHON_ULTRA_HH
#define TOMBEXCAVATOR_ARCHON_ULTRA_HH

#include <string>
#include <map>
#include <mio/mio.hpp>

namespace archon_ultra
{
    class pkg_file
    {
    public:
        pkg_file(const std::string& path);
    private:
        mio::mmap_source m_source;
    };
}


#endif //TOMBEXCAVATOR_ARCHON_ULTRA_HH
