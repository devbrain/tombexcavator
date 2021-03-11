//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_SPRITE_GROUP_EXPORT_HH
#define TOMBEXCAVATOR_SPRITE_GROUP_EXPORT_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/export/transformer.hh>
#include <tomb-excavator/provider/dto/video/sprite.hh>
#include <tomb-excavator/provider/dto/video/coord.hh>
#include <vector>
#include <filesystem>

namespace exporter
{
    class EXPORT_API tile_sheet
    {
    public:
        struct sprite_info
        {
            sprite_info(int x, int y, int w, int h, int id);
            provider::dto::coord m_coord;
            int                  m_id;
        };
        using sprites_vec_t = std::vector<sprite_info>;
        using png_t = std::vector <char>;
    public:
        explicit tile_sheet(const provider::dto::sprite_group& sg);

        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] std::size_t size() const noexcept;
        [[nodiscard]] sprites_vec_t::const_iterator begin() const;
        [[nodiscard]] sprites_vec_t::const_iterator end() const;

        [[nodiscard]] const png_t& png() const noexcept;
        [[nodiscard]] provider::dto::dimension dim() const noexcept;
    private:
        static provider::dto::dimension eval_dim(const provider::dto::sprite_group& sg);
    private:
        sprites_vec_t            m_sprites;
        png_t                    m_png;
        provider::dto::dimension m_dim;
    };
    // ==============================================================================
    EXPORT_API void to_png(const tile_sheet& ts, const std::filesystem::path& oname);

    EXPORT_API void to_tsx(const tile_sheet& ts,
                           const std::string& tileset_name,
                           const std::filesystem::path& oname_tmx,
                           const std::filesystem::path& oname_png);
}

#endif //TOMBEXCAVATOR_SPRITE_GROUP_EXPORT_HH
