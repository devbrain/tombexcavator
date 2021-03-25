//
// Created by igor on 11/03/2021.
//

#include <tomb-excavator/games/common/vfs/virtual_fs.hh>

namespace games::common
{
    virtual_fs::virtual_fs(const std::string& name, const root_directory::loaders_list_t& loaders)
    : m_name(name),
      m_root(loaders)
    {

    }
    // --------------------------------------------------------------------------------------------------------
    std::string virtual_fs::name() const
    {
        return m_name;
    }
    // --------------------------------------------------------------------------------------------------------
    bool virtual_fs::accept(const provider::physfs::directory& dir) const
    {
        return m_root.accept(dir);
    }
    // --------------------------------------------------------------------------------------------------------
    std::unique_ptr<provider::vfs::directory> virtual_fs::root(const provider::physfs::directory& dir) const
    {
        auto cloned = m_root;
        cloned.open(dir);
        return std::make_unique<root_directory>(cloned);
    }
}
