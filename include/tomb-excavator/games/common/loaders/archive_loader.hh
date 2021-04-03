//
// Created by igor on 30/03/2021.
//

#ifndef TOMBEXCAVATOR_ARCHIVE_LOADER_HH
#define TOMBEXCAVATOR_ARCHIVE_LOADER_HH

#include <tomb-excavator/games/common/loaders/base_archive_loader.hh>
#include <variant>
#include <optional>
#include <cstdint>
#include <tuple>

namespace games::common
{
    // -----------------------------------------------------------------------------
    class GAMES_COMMON_API archive_loader : public base_archive_data_loader
    {
    public:
        explicit archive_loader(std::string physical_name, std::unique_ptr<archive_entry_loaders_registry> loaders);
    protected:
        struct fat_file_event
        {
            std::string name;
            std::size_t size;
            uint64_t    offset; // absolute offset
            std::optional<fat_entry_t::props_map_t> props;
        };

        struct fat_dir_start_event
        {
            std::string name;
            std::optional<fat_entry_t::props_map_t> props;
        };

        struct fat_dir_end_event { };

        using fat_event_t = std::variant<fat_file_event, fat_dir_start_event, fat_dir_end_event>;

        [[nodiscard]] virtual std::optional<fat_event_t> read_next_entry(std::istream& is) = 0;
        virtual void read_header(std::istream& is) = 0;

    private:
        void load_fat(std::istream& is, fat_events& builder) override;
    };
}

#endif //TOMBEXCAVATOR_ARCHIVE_LOADER_HH
