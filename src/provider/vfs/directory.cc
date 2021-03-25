//
// Created by igor on 21/02/2021.
//

#include <tomb-excavator/provider/vfs/directory.hh>

namespace provider::vfs
{
    directory::directory()
    : m_parent(nullptr)
    {

    }
    // ---------------------------------------------------------------------------------------------------------
    directory::~directory() = default;
    // ---------------------------------------------------------------------------------------------------------
    std::unique_ptr<directory> directory::open_directory(std::size_t entry_idx)
    {
        auto ret = this->load_directory(entry_idx);
        ret->m_parent = this;
        return ret;
    }
} // ns
