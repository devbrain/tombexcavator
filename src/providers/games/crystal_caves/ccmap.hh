//
// Created by igor on 15/03/2021.
//

#ifndef TOMBEXCAVATOR_CCMAP_HH
#define TOMBEXCAVATOR_CCMAP_HH

#include <vector>
#include <string>
#include <tomb-excavator/games/common/loaders/data_loader.hh>

class ccmap : public games::common::data_loader
{
public:
    explicit ccmap(std::string phys_name);
private:
    void open(std::shared_ptr<provider::physfs::directory> dir) override;

    [[nodiscard]] std::size_t size() const override;
    [[nodiscard]] provider::file_type_t load(std::size_t index) const override;
    [[nodiscard]] std::string name(std::size_t index) const override;
private:
    using level_t = std::vector<std::string>;
    std::vector<level_t> m_levels;
};

#endif //TOMBEXCAVATOR_CCMAP_HH
