//
// Created by igor on 11/03/2021.
//

#include <algorithm>
#include <tomb-excavator/games/common/vfs/root_directory.hh>
#include "directory_adaptor.hh"

namespace games::common
{
    root_directory::root_directory(const loaders_list_t& loaders)
    {
        for (auto* dl : loaders)
        {
            m_loaders.emplace_back(dl);
        }
    }
    // ----------------------------------------------------------------------------------------------
    root_directory::root_directory(const root_directory& other) = default;
    // ----------------------------------------------------------------------------------------------
    bool root_directory::accept(const provider::physfs::directory& dir) const
    {
        return std::all_of(m_loaders.begin(), m_loaders.end(),
                           [&dir](const auto& dl) {return dl->accept(dir);});
    }
    // ----------------------------------------------------------------------------------------------
    void root_directory::open(const provider::physfs::directory& dir)
    {
        m_dir = std::make_shared<provider::physfs::directory>(dir);

        for (std::size_t loader_idx = 0; loader_idx < m_loaders.size(); loader_idx++)
        {
            auto& dl = m_loaders[loader_idx];
            dl->open(m_dir);
            const auto entries = dl->size();
            for (std::size_t e = 0; e<entries; e++)
            {
                m_index.emplace_back(loader_idx, e);
            }
        }
    }
    // ----------------------------------------------------------------------------------------------
    std::size_t root_directory::entries() const
    {
        return m_index.size();
    }
    // ----------------------------------------------------------------------------------------------
    std::string root_directory::name(std::size_t entry_idx) const
    {
        const auto& e = m_index[entry_idx];
        return m_loaders[e.m_loader_index]->name(e.m_entry_index);
    }
    // ----------------------------------------------------------------------------------------------
    bool root_directory::is_directory(std::size_t entry_idx) const
    {
        const auto& e = m_index[entry_idx];
        return m_loaders[e.m_loader_index]->is_directory(e.m_entry_index);
    }
    // ----------------------------------------------------------------------------------------------
    std::unique_ptr<provider::vfs::directory> root_directory::load_directory(std::size_t entry_idx) const
    {
        const auto& e = m_index[entry_idx];
        std::shared_ptr<abstract_directory> ad = m_loaders[e.m_loader_index]->load_directory(e.m_entry_index);
        return std::make_unique<directory_adaptor>(ad);
    }
    // ----------------------------------------------------------------------------------------------
    provider::file_content_t root_directory::open_file(std::size_t entry_idx) const
    {
        const auto& e = m_index[entry_idx];
        return m_loaders[e.m_loader_index]->load(e.m_entry_index);
    }
    // ----------------------------------------------------------------------------------------------
    provider::file_type_t root_directory::file_type(std::size_t entry_idx) const
    {
        const auto& e = m_index[entry_idx];
        return m_loaders[e.m_loader_index]->type(e.m_entry_index);
    }
}
