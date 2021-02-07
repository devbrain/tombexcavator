//
// Created by igor on 16/05/2020.
//

#ifndef TOMBEXCAVATOR_STARGUNNER_HH
#define TOMBEXCAVATOR_STARGUNNER_HH

#include <vector>
#include <string>
#include <map>

#include "../../archive/io.hh"
#include <mio/mio.hpp>
#include "../../archive/tile_sheet.hh"

namespace stargunner
{
    class fat_entry
    {
    public:
        fat_entry(std::size_t size, std::size_t out_size, const std::byte* data) noexcept;
        fat_entry() noexcept;
        fat_entry(const fat_entry&) = default;
        bool is_compressed() const noexcept ;
        bool is_directory() const noexcept ;
        std::size_t size() const noexcept ;
        std::size_t out_size() const noexcept ;
        const std::byte* stream () const noexcept ;
    private:
        std::size_t m_size;
        std::size_t m_out_size;
        const std::byte* m_stream;
    };

    class dlt_archive
    {
    public:
        dlt_archive (const std::string& path);

        std::map<std::string, fat_entry> scan(const std::string& directory) const;
        static void decompress(const fat_entry& fe, std::vector<std::byte>& out);
        fat_entry get_entry(const std::string& name);

    private:
        mio::mmap_source m_source;
        std::map<std::string, fat_entry> m_fat;
    };

    std::vector<std::vector<int16_t >> load_animation_sequences(archive::input& input);
    archive::palette load_palette(archive::input& input);
    archive::tile_sheet load_tile_sheet(const std::vector<std::vector<int16_t >>& groups, const archive::palette& pal,
                                        archive::input& input);
    std::vector <int> load_frames(archive::input& input);
    void dump_sprites(archive::input& input, const archive::palette& pal, const std::string& oname);
}


#endif //TOMBEXCAVATOR_STARGUNNER_HH
