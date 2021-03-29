//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_SPRITE_HH
#define TOMBEXCAVATOR_SPRITE_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/dto/video/canvas.hh>
#include <tomb-excavator/provider/dto/video/bitmask.hh>
#include <tomb-excavator/provider/dto/video/palette.hh>

#include <vector>
#include <tuple>
#include <optional>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace provider::dto
{
    class PROVIDER_API sprite
    {
    public:
        sprite(int w, int h, bool is_masked = false);
        explicit sprite(const dimension& dim, bool is_masked = false);

        [[nodiscard]] const std::optional<bitmask>& get_bitmask() const noexcept;
        [[nodiscard]] std::optional<bitmask>& get_bitmask() noexcept;

        [[nodiscard]] const canvas& get_canvas() const noexcept;
        [[nodiscard]] canvas& get_canvas() noexcept;

        [[nodiscard]] const dimension& dim() const noexcept;

        void set_id(int id);
        [[nodiscard]] std::optional<int> get_id() const noexcept;
    private:
        canvas m_canvas;
        std::optional<bitmask> m_bitmask;
        std::optional<int> m_id;
    };

    class PROVIDER_API sprite_group
    {
    public:
        using container_t = std::vector<sprite>;
        using iterator = container_t::iterator;
        using const_iterator = container_t::const_iterator;
    public:

        static sprite_group split_by_index(const sprite_group& origin, const std::vector<std::size_t>& indexes);

        sprite_group() = default;
        sprite_group(const sprite_group&) = default;

        [[nodiscard]] iterator begin();
        [[nodiscard]] iterator end();

        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;

        [[nodiscard]] sprite& add(int w, int h, bool is_masked = false);
        [[nodiscard]] sprite& add(const dimension& dim, bool is_masked = false);

        [[nodiscard]] const palette& pal() const;
        [[nodiscard]] palette& pal();

        [[nodiscard]] bool empty() const;
        [[nodiscard]] std::size_t size() const;

        const sprite& operator[](std::size_t k) const;
        sprite& operator[](std::size_t k);

    private:
        palette m_pal;
        container_t m_sprites;
    };



    class PROVIDER_API tile_sheet
    {
    public:
        struct sprite_info
        {
            sprite_info(int x, int y, int w, int h, int id);
            provider::dto::coord m_coord;
            int m_id;
        };

        using sprites_vec_t = std::vector<sprite_info>;
        using png_t = std::vector<char>;
    public:
        static std::vector<tile_sheet> create(const provider::dto::sprite_group& sg);

        explicit tile_sheet(const provider::dto::sprite_group& sg);

        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] std::size_t size() const noexcept;
        [[nodiscard]] sprites_vec_t::const_iterator begin() const;
        [[nodiscard]] sprites_vec_t::const_iterator end() const;

        [[nodiscard]] const png_t& png() const noexcept;
        [[nodiscard]] dimension dim() const noexcept;
        [[nodiscard]] dimension sprite_dim() const noexcept;
    private:

        static std::tuple<dimension,dimension> eval_dim(const provider::dto::sprite_group& sg);
    private:
        sprites_vec_t m_sprites;
        png_t m_png;
        std::tuple<dimension, dimension> m_dims;
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_SPRITE_HH
