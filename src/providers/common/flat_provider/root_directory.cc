//
// Created by igor on 11/03/2021.
//

#include <tomb-excavator/games/common/flat_provider/root_directory.hh>

namespace games::common
{
    root_directory::root_directory(const loaders_list_t& loaders)
    : m_dir (nullptr)
    {
        for (data_loader* dl : loaders)
        {
            m_loaders.emplace_back(dl);
        }
    }
    // ----------------------------------------------------------------------------------------------
    root_directory::root_directory(const root_directory& other) = default;
    // ----------------------------------------------------------------------------------------------
    bool root_directory::accept(const provider::physfs::directory& dir) const
    {
        for (const auto& dl : m_loaders)
        {
            if (!dir.contains(dl->physical_name()))
            {
                return false;
            }
        }
        return true;
    }
    // ----------------------------------------------------------------------------------------------
    void root_directory::open(const provider::physfs::directory& dir)
    {
        m_dir = &dir;
    }
    // ----------------------------------------------------------------------------------------------
    std::size_t root_directory::entries() const
    {
        return m_loaders.size();
    }
    // ----------------------------------------------------------------------------------------------
    std::string root_directory::name(std::size_t entry_idx) const
    {
        return m_loaders[entry_idx]->name();
    }
    // ----------------------------------------------------------------------------------------------
    bool root_directory::is_directory([[maybe_unused]] std::size_t entry_idx) const
    {
        return false;
    }
    // ----------------------------------------------------------------------------------------------
    std::unique_ptr<provider::vfs::directory> root_directory::load_directory([[maybe_unused]] std::size_t entry_idx) const
    {
        return nullptr;
    }
    // ----------------------------------------------------------------------------------------------
    provider::file_type_t root_directory::open_file(std::size_t entry_idx) const
    {
        return m_loaders[entry_idx]->load(*m_dir);
    }
}