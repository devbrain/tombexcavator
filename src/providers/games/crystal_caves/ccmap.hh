//
// Created by igor on 15/03/2021.
//

#ifndef TOMBEXCAVATOR_CCMAP_HH
#define TOMBEXCAVATOR_CCMAP_HH

#include <vector>
#include <string>
#include <tomb-excavator/games/common/loaders/archive_loader.hh>
#include <tomb-excavator/bsw/io/binary_reader.hh>

class ccmap : public games::common::archive_data_loader
{
public:
    explicit ccmap(std::string phys_name);
private:
    [[nodiscard]] std::vector<fat_entry> load_fat(std::istream& is) override;
    static loaders_map_t loaders();
private:
    std::unique_ptr<bsw::io::binary_reader> m_exploded_exe;
private:
    using level_t = std::vector<std::string>;
    std::vector<level_t> m_levels;
};

#endif //TOMBEXCAVATOR_CCMAP_HH
