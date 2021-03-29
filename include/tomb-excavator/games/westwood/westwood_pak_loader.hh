//
// Created by igor on 25/03/2021.
//

#ifndef TOMBEXCAVATOR_WESTWWOD_PAK_LOADER_HH
#define TOMBEXCAVATOR_WESTWWOD_PAK_LOADER_HH

#include <tomb-excavator/export-games-westwood.h>
#include <tomb-excavator/games/common/loaders/archive_loader.hh>
#include <tomb-excavator/games/common/loaders/loader_functions.hh>

#include <map>
#include <string>
#include <tuple>

namespace games::westwood
{
    class GAMES_WESTWOOD_API pak_loader : public common::archive_data_loader
    {
    public:
        pak_loader(std::string phys_name, const loaders_map_t& loaders);
    protected:
        // type, props, internal type
        using file_data_t = std::tuple<provider::file_type_t, fat_entry::props_map_t, int>;
        virtual std::optional<file_data_t> get_file_data(const std::string& name) = 0;
    private:
        [[nodiscard]] std::vector<fat_entry> load_fat(std::istream& is) override;

        provider::file_type_t deduce_file_data(const std::string& name,
                                               common::archive_data_loader::fat_entry::props_map_t& props,
                                               int& internal_type);

    private:
        static loaders_map_t eval_loaders (const loaders_map_t& loaders);
    };
}

#endif //TOMBEXCAVATOR_WESTWWOD_PAK_LOADER_HH
