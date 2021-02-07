//
// Created by igor on 27/05/2020.
//

#ifndef ARCHIVE_TILE_SHEET_HH
#define ARCHIVE_TILE_SHEET_HH

#include <vector>
#include <string>
#include <archive/archive_api.h>
#include "coord.hh"
#include "palette.hh"
#include "canvas.hh"

namespace archive
{
    class ARCHIVE_API sprite_group
    {
    public:
        sprite_group(std::vector<coord>&& data);
    private:
        std::vector<coord> m_sprites;
    };

    class ARCHIVE_API tile_sheet
    {
    public:
        tile_sheet(std::vector<sprite_group>&& groups, canvas&& image);

        bool save_to_png(const std::string& oname);
    private:
        std::vector<sprite_group> m_groups;
        canvas m_canvas;
    };

    class ARCHIVE_API tile_sheet_builder
    {
    public:
        tile_sheet_builder () = default;

        tile_sheet build();
        tile_sheet_builder& add_palette(const palette& pal);
        tile_sheet_builder& start_group();
        tile_sheet_builder& add_sprite(std::vector<unsigned char>&& data, int w, int h);
        tile_sheet_builder& end_group();
    private:
        palette m_palette;
        struct sprite_data
        {
            sprite_data (std::size_t index_, int w, int h);
            std::size_t index;
            dimension dim;
        };
        std::vector<sprite_data> m_current_group;
        std::vector<std::vector<sprite_data>> m_sprite_groups;
        std::vector<std::vector<unsigned char>> m_sprites;
    };
}

#endif //TOMBEXCAVATOR_TILE_SHEET_HH