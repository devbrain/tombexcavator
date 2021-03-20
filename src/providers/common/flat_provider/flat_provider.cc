//
// Created by igor on 11/03/2021.
//

#include <tomb-excavator/games/common/flat_provider/flat_provider.hh>

namespace games::common
{
    flat_provider::flat_provider(const std::string& name, const root_directory::loaders_list_t& loaders)
    : m_name(name),
      m_root(this, loaders)
    {

    }
    // --------------------------------------------------------------------------------------------------------
    std::string flat_provider::name() const
    {
        return m_name;
    }
    // --------------------------------------------------------------------------------------------------------
    bool flat_provider::accept(const provider::physfs::directory& dir) const
    {
        return m_root.accept(dir);
    }
    // --------------------------------------------------------------------------------------------------------
    std::unique_ptr<provider::vfs::directory> flat_provider::root(const provider::physfs::directory& dir) const
    {
        auto cloned = m_root;
        cloned.open(dir);
        return std::make_unique<root_directory>(cloned);
    }
}
