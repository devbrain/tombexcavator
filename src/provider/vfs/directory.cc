//
// Created by igor on 21/02/2021.
//

#include <tomb-excavator/provider/vfs/directory.hh>

namespace provider::vfs
{
    directory::directory(file_system* owner)
    : m_owner(owner)
    {

    }
    // ---------------------------------------------------------------------------------------------------------
    directory::~directory() = default;
    // ---------------------------------------------------------------------------------------------------------
    file_system* directory::owner()
    {
        return m_owner;
    }
} // ns
