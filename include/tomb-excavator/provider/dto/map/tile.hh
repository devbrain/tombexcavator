//
// Created by igor on 15/03/2021.
//

#ifndef TOMBEXCAVATOR_DTO_TILE_HH
#define TOMBEXCAVATOR_DTO_TILE_HH

#include <tomb-excavator/export-provider.h>
#include <vector>
#include <string>
#include <tuple>
#include <tomb-excavator/msvc/c4251-begin.h>
namespace provider::dto
{

    class PROVIDER_API tile
    {
    public:
        using name_value_t = std::tuple<std::string, std::string>;
        using ilist_t = std::initializer_list<name_value_t>;
        using props_t = std::vector<name_value_t>;
        using const_iterator = props_t::const_iterator ;
    public:
        explicit tile(int id);
        tile (int id, ilist_t props);

        [[nodiscard]] int get_id() const noexcept;
        [[nodiscard]] bool empty() const noexcept;
        [[nodiscard]] const_iterator begin() const;
        [[nodiscard]] const_iterator end() const;
    private:
        int m_id;
        props_t m_props;
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_TILE_HH
