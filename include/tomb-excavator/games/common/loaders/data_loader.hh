//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_DATA_LOADER_HH
#define TOMBEXCAVATOR_DATA_LOADER_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/physfs/directory.hh>
#include <tomb-excavator/provider/file_types.hh>

namespace games::common
{
    class GAMES_COMMON_API data_loader
    {
    public:
        data_loader(std::string virt_name, std::string phys_name);
        virtual ~data_loader();

        [[nodiscard]] provider::file_type_t load(const provider::physfs::directory& dir) const;
        [[nodiscard]] const std::string& name() const;
        [[nodiscard]] const std::string& physical_name() const;
    protected:
        virtual provider::file_type_t read(std::istream& is) const = 0;
    private:
        std::string m_virt_name;
        std::string m_phys_name;
    };
}

#endif //TOMBEXCAVATOR_DATA_LOADER_HH
