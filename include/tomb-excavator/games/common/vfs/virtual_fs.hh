//
// Created by igor on 11/03/2021.
//

#ifndef TOMBEXCAVATOR_VIRTUAL_FS_HH
#define TOMBEXCAVATOR_VIRTUAL_FS_HH

#include <tomb-excavator/games/common/vfs/root_directory.hh>
#include <tomb-excavator/provider/vfs/file_system.hh>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::common
{
    class GAMES_COMMON_API virtual_fs : public provider::vfs::file_system
    {
    public:
        virtual_fs(const std::string& name, const root_directory::loaders_list_t& loaders);
    private:
        [[nodiscard]] std::string name() const override;
        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const override;
        [[nodiscard]] std::unique_ptr<provider::vfs::directory> root(const provider::physfs::directory& dir) const override;
    private:
        std::string m_name;
        root_directory m_root;
    };
}

#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_VIRTUAL_FS_HH
