//
// Created by igor on 22/03/2021.
//

#ifndef TOMBEXCAVATOR_BASE_ARCHIVE_LOADER_HH
#define TOMBEXCAVATOR_BASE_ARCHIVE_LOADER_HH

#include <variant>
#include <tuple>
#include <vector>
#include <cstdint>
#include <functional>

#include <tomb-excavator/games/common/vfs/data_loader.hh>
#include <tomb-excavator/games/common/loaders/fat.hh>
#include <tomb-excavator/games/common/loaders/archive_entry_loader.hh>

#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::common
{
    /**
     * Loader for archives
     */
    class GAMES_COMMON_API base_archive_data_loader : public physical_data_loader
    {
    public:
        using fat_entry_t = fat_entry;
    public:
        explicit base_archive_data_loader(std::string phys_name, std::unique_ptr<archive_entry_loaders_registry> loaders);
    protected:
        virtual void load_fat(std::istream& is, fat_events& builder) = 0;
    private:
        void open(std::shared_ptr<provider::physfs::directory> dir) override;

        [[nodiscard]] std::size_t size() const override;
        [[nodiscard]] provider::file_content_t load(std::size_t index) const override;
        [[nodiscard]] provider::file_type_t    type(std::size_t index) const override;
        [[nodiscard]] std::string name(std::size_t index) const override;
        [[nodiscard]] bool is_directory(std::size_t index) const override;
        [[nodiscard]] std::shared_ptr<abstract_directory> load_directory(std::size_t index) const override;
    protected:
        archive_entry_loaders_registry& loaders ();
        const archive_entry_loaders_registry& loaders () const;
    private:
        std::shared_ptr<abstract_directory> m_root;
        std::vector<fat_entry> m_fat;
        std::shared_ptr<archive_entry_loaders_registry> m_loaders;
    };
}

#include <tomb-excavator/msvc/c4251-end.h>

#endif //TOMBEXCAVATOR_BASE_ARCHIVE_LOADER_HH
