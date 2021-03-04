//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_DATA_LOADER_HH
#define TOMBEXCAVATOR_DATA_LOADER_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/physfs/directory.hh>
#include <tomb-excavator/provider/file_types.hh>

namespace provider::physfs
{
    class PROVIDER_API data_loader
    {
    public:
        data_loader(std::string virt_name, std::string phys_name, const directory& dir);
        virtual ~data_loader();

        file_type_t load() const;
        const std::string& name() const;
    protected:
        virtual file_type_t read(std::istream& is) const = 0;
    private:
        std::string m_virt_name;
        std::string m_phys_name;
        const directory& m_dir;
    };
}

#endif //TOMBEXCAVATOR_DATA_LOADER_HH
