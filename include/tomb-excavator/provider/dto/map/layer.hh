//
// Created by igor on 15/03/2021.
//

#ifndef TOMBEXCAVATOR_LAYER_HH
#define TOMBEXCAVATOR_LAYER_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/dto/map/tile.hh>
#include <tomb-excavator/provider/dto/video/coord.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <vector>

namespace provider::dto
{
    class PROVIDER_API layer
    {
    public:
        layer (int w, int h);
        explicit layer (dimension dims);

        [[nodiscard]] dimension dim() const noexcept;


        void put_tile(int x, int y, int id, tile::ilist_t props)
        {
            if (x >= m_dim.w())
            {
                RAISE_EX(x, ">=", m_dim.w());
            }
            if ( y >= m_dim.h())
            {
                RAISE_EX(y, ">=", m_dim.h());
            }
            m_tiles[y*m_dim.w() + x] = tile(id, props);
        }

        [[nodiscard]] const tile& get_tile(int x, int y) const;
    private:
        dimension m_dim;
        std::vector<tile> m_tiles;
    };
}

#endif //TOMBEXCAVATOR_LAYER_HH
