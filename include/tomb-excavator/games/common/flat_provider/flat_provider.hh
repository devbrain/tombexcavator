//
// Created by igor on 11/03/2021.
//

#ifndef TOMBEXCAVATOR_FLAT_PROVIDER_HH
#define TOMBEXCAVATOR_FLAT_PROVIDER_HH

#include <tomb-excavator/games/common/flat_provider/root_directory.hh>
#include <tomb-excavator/provider/vfs/file_system.hh>

namespace games::common
{
    class GAMES_COMMON_API flat_provider : public provider::vfs::file_system
    {
    public:
        flat_provider(const std::string& name, const root_directory::loaders_list_t& loaders);
    private:
        [[nodiscard]] std::string name() const override;
        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
        [[nodiscard]] std::unique_ptr<provider::vfs::directory> root(const provider::physfs::directory& dir) const override;
    private:
        std::string m_name;
        root_directory m_root;
    };
}

#endif //TOMBEXCAVATOR_FLAT_PROVIDER_HH
