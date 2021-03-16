//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/loaders/data_loader.hh>

namespace games::common
{
    data_loader::data_loader(std::string phys_name)
    : m_phys_name(std::move(phys_name))
    {

    }
    // ----------------------------------------------------------------------------------------------
    data_loader::~data_loader() = default;
    // ----------------------------------------------------------------------------------------------
    const std::string& data_loader::physical_name() const
    {
        return m_phys_name;
    }
    // ==============================================================================================
    single_entry_data_loader::single_entry_data_loader(std::string virt_name, std::string phys_name)
    : data_loader(std::move(phys_name)),
      m_virt_name(std::move(virt_name))
    {

    }
    // --------------------------------------------------------------------------------------------
    void single_entry_data_loader::open(std::shared_ptr<provider::physfs::directory> dir)
    {
        m_dir = dir;
    }
    // --------------------------------------------------------------------------------------------
    provider::file_type_t single_entry_data_loader::load([[maybe_unused]] std::size_t index) const
    {
        auto is = m_dir->open_file(physical_name());
        return read(*is);
    }
    // --------------------------------------------------------------------------------------------
    std::string single_entry_data_loader::name([[maybe_unused]] std::size_t index) const
    {
        return m_virt_name;
    }
    // --------------------------------------------------------------------------------------------
    std::size_t single_entry_data_loader::size() const
    {
        return 1;
    }
}