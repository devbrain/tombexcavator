//
// Created by igor on 25/03/2021.
//

#include "directory_adaptor.hh"

namespace games::common
{
    directory_adaptor::directory_adaptor(std::shared_ptr<abstract_directory> ad)
    : m_adapted(ad)
    {

    }
    // ----------------------------------------------------------------------------------------------------
    std::size_t directory_adaptor::entries() const
    {
        return m_adapted->size();
    }
    // ----------------------------------------------------------------------------------------------------
    std::string directory_adaptor::name(std::size_t entry_idx) const
    {
        return m_adapted->name(entry_idx);
    }
    // ----------------------------------------------------------------------------------------------------
    bool directory_adaptor::is_directory(std::size_t entry_idx) const
    {
        return m_adapted->is_directory(entry_idx);
    }
    // ----------------------------------------------------------------------------------------------------
    provider::file_content_t directory_adaptor::open_file(std::size_t entry_idx) const
    {
        return m_adapted->load(entry_idx);
    }
    // ----------------------------------------------------------------------------------------------------
    provider::file_type_t directory_adaptor::file_type(std::size_t entry_idx) const
    {
        return m_adapted->type(entry_idx);
    }
    // ----------------------------------------------------------------------------------------------------
    std::unique_ptr<provider::vfs::directory> directory_adaptor::load_directory(std::size_t entry_idx) const
    {
        return std::make_unique<directory_adaptor>(m_adapted->load_directory(entry_idx));
    }
}
