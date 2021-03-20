//
// Created by igor on 15/03/2021.
//

#ifndef TOMBEXCAVATOR_MAP_HH
#define TOMBEXCAVATOR_MAP_HH

#include <tomb-excavator/export-provider.h>
#include <tomb-excavator/provider/dto/map/layer.hh>
#include <tomb-excavator/provider/dto/video/coord.hh>
#include <tomb-excavator/provider/dto/video/sprite.hh>

#include <vector>
#include <tuple>

namespace provider::dto
{
    class PROVIDER_API basic_map
    {
    public:
        using layers_t = std::vector<layer>;
        using const_iterator = layers_t::const_iterator ;
    public:
        basic_map(int w, int h);
        explicit basic_map(dimension d);

        [[nodiscard]] layer& add_layer();
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
    private:
        dimension m_dim;
        layers_t  m_layers;
    };

    using map = std::tuple<basic_map, sprite_group>;

}

#endif //TOMBEXCAVATOR_MAP_HH
