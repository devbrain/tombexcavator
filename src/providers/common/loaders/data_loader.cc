//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/loaders/data_loader.hh>

namespace games::common
{
    data_loader::data_loader(std::string virt_name, std::string phys_name)
    : m_virt_name(std::move(virt_name)),
      m_phys_name(std::move(phys_name))
    {

    }
    // --------------------------------------------------------------------------------------------
    data_loader::~data_loader() = default;
    // --------------------------------------------------------------------------------------------
    provider::file_type_t data_loader::load(const provider::physfs::directory& dir) const
    {
        auto is = dir.open_file(m_phys_name);
        return read(*is);
    }
    // --------------------------------------------------------------------------------------------
    const std::string& data_loader::name() const
    {
        return m_virt_name;
    }
    // --------------------------------------------------------------------------------------------
    const std::string& data_loader::physical_name() const
    {
        return m_phys_name;
    }
}