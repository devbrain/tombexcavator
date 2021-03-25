//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/vfs/data_loader.hh>


namespace games::common
{
    abstract_directory::~abstract_directory() = default;
    // =============================================================================================
    physical_data_loader::physical_data_loader(std::string phys_name)
            : m_phys_name(std::move(phys_name))
    {

    }
    // ----------------------------------------------------------------------------------------------
    bool physical_data_loader::accept(const provider::physfs::directory& dir) const
    {
        return dir.contains(m_phys_name);
    }
    // ----------------------------------------------------------------------------------------------
    const std::string& physical_data_loader::physical_name() const
    {
        return m_phys_name;
    }
    // =============================================================================================
    base_single_entry_data_loader::base_single_entry_data_loader(std::string virt_name)
            : m_virt_name(std::move(virt_name))
    {

    }
    // --------------------------------------------------------------------------------------------
    std::size_t base_single_entry_data_loader::size() const
    {
        return 1;
    }
    // --------------------------------------------------------------------------------------------
    bool base_single_entry_data_loader::is_directory([[maybe_unused]] std::size_t index) const
    {
        return false;
    }
    // --------------------------------------------------------------------------------------------
    std::string base_single_entry_data_loader::name([[maybe_unused]] std::size_t index) const
    {
        return m_virt_name;
    }
    // --------------------------------------------------------------------------------------------
    std::shared_ptr<abstract_directory> base_single_entry_data_loader::load_directory(std::size_t index) const
    {
        return nullptr;
    }

}