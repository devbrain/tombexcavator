//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/provider/physfs/data_loader.hh>

namespace provider::physfs
{
    data_loader::data_loader(std::string virt_name, std::string phys_name, const directory& dir)
    : m_virt_name(std::move(virt_name)),
      m_phys_name(std::move(phys_name)),
      m_dir(dir)
    {

    }
    // --------------------------------------------------------------------------------------------
    data_loader::~data_loader() = default;
    // --------------------------------------------------------------------------------------------
    file_type_t data_loader::load() const
    {
        auto is = m_dir.open_file(m_phys_name);
        return read(*is);
    }
    // --------------------------------------------------------------------------------------------
    const std::string& data_loader::name() const
    {
        return m_virt_name;
    }
}