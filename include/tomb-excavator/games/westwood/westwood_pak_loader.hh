//
// Created by igor on 25/03/2021.
//

#ifndef TOMBEXCAVATOR_WESTWWOD_PAK_LOADER_HH
#define TOMBEXCAVATOR_WESTWWOD_PAK_LOADER_HH

#include <tomb-excavator/export-games-westwood.h>
#include <tomb-excavator/games/common/loaders/archive_loader.hh>

#include <map>
#include <string>
#include <tuple>

namespace games::westwood
{
    class GAMES_WESTWOOD_API pak_loader : public common::archive_loader
    {
    public:
        pak_loader(std::string phys_name, std::initializer_list<common::archive_entry_loader> loaders);
    protected:
        virtual void setup_loader_context(const std::string& entry_name, fat_entry_t::props_map_t& props, fat_entry_t::dependencies_t& deps) = 0;
    private:
        [[nodiscard]] std::optional<fat_event_t> read_next_entry(std::istream& is) override;
        void read_header(std::istream& is) override;
    private:
        uint64_t m_current_offs;
        uint64_t m_file_size;
        fat_file_event m_prev_event;
        bool m_is_first;
        bool m_is_last;
    };
}

#endif //TOMBEXCAVATOR_WESTWWOD_PAK_LOADER_HH
