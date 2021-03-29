//
// Created by igor on 25/03/2021.
//

#include <tomb-excavator/games/westwood/westwood_data_loader.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <algorithm>

namespace games::westwood
{
    root_directory_loader::root_directory_loader(file_names_t pak_files)
    {
        for (const auto& pak_name : pak_files)
        {
            m_archive_loaders.emplace_back(pak_name, nullptr);
        }
    }
    // ------------------------------------------------------------------------------------------------------
    std::size_t root_directory_loader::size() const
    {
        return m_archive_loaders.size();
    }
    // ------------------------------------------------------------------------------------------------------
    provider::file_content_t root_directory_loader::load([[maybe_unused]] std::size_t index) const
    {
        RAISE_EX("Should not be here");
    }
    // ------------------------------------------------------------------------------------------------------
    provider::file_type_t root_directory_loader::type([[maybe_unused]] std::size_t index) const
    {
        RAISE_EX("Should not be here");
    }
    // ------------------------------------------------------------------------------------------------------
    std::string root_directory_loader::name(std::size_t index) const
    {
        const auto& name = std::get<0>(m_archive_loaders[index]);
        auto idx = name.rfind('.');
        return name.substr(0, idx);
    }
    // ------------------------------------------------------------------------------------------------------
    bool root_directory_loader::is_directory([[maybe_unused]] std::size_t index) const
    {
        return true;
    }
    // ------------------------------------------------------------------------------------------------------
    std::shared_ptr<common::abstract_directory> root_directory_loader::load_directory(std::size_t index) const
    {
        return  std::get<1>(m_archive_loaders[index]);
    }
    // ------------------------------------------------------------------------------------------------------
    void root_directory_loader::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        for (auto& e : m_archive_loaders)
        {
            if (!std::get<1>(e))
            {
                auto ldr = create_pak_loader(std::get<0>(e));
                ldr->open(dir);
                std::get<1>(e) = std::move(ldr);
            }
        }
    }
    // ------------------------------------------------------------------------------------------------------
    bool root_directory_loader::accept(const provider::physfs::directory& dir) const
    {
        return std::all_of(m_archive_loaders.begin(), m_archive_loaders.end(),
                           [&dir](const auto& elt) {return dir.contains(std::get<0>(elt));});
    }
}