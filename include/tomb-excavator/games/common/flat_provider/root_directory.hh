//
// Created by igor on 11/03/2021.
//

#ifndef TOMBEXCAVATOR_FLAT_ROOT_DIRECTORY_HH
#define TOMBEXCAVATOR_FLAT_ROOT_DIRECTORY_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/games/common/loaders/data_loader.hh>
#include <tomb-excavator/provider/vfs/directory.hh>

namespace games::common
{
    class GAMES_COMMON_API root_directory : public provider::vfs::directory
    {
    public:
        using loaders_list_t = std::initializer_list<data_loader*>;
    public:
        root_directory(const loaders_list_t& loaders);
        root_directory(const root_directory& other);

        [[nodiscard]] bool accept(const provider::physfs::directory& dir) const;
        void open(const provider::physfs::directory& dir);
    private:
        [[nodiscard]] std::size_t entries() const override;
        [[nodiscard]] std::string name(std::size_t entry_idx) const override;
        [[nodiscard]] bool is_directory([[maybe_unused]] std::size_t entry_idx) const override;
        [[nodiscard]] std::unique_ptr<provider::vfs::directory> load_directory([[maybe_unused]] std::size_t entry_idx) const override;
        [[nodiscard]] provider::file_type_t open_file(std::size_t entry_idx) const override;
    private:
        std::vector<std::shared_ptr<games::common::data_loader>> m_loaders;
        const provider::physfs::directory* m_dir;
    };
}


#endif