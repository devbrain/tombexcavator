//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/loaders/data_loader.hh>


namespace games::common
{
    abstract_data_loader::~abstract_data_loader() = default;
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


}