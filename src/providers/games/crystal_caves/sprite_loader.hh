//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_SPRITE_LOADER_HH
#define TOMBEXCAVATOR_SPRITE_LOADER_HH

#include <tomb-excavator/provider/physfs/data_loader.hh>

class sprite_loader : public provider::physfs::data_loader
{
public:
    sprite_loader(std::string virt_name, std::string phys_name, const provider::physfs::directory& dir, int first_sprite_id)
    : provider::physfs::data_loader(virt_name, phys_name, dir),
    m_first_sprite_id(first_sprite_id)
    {}
private:
    int m_first_sprite_id;
    provider::file_type_t read(std::istream& is) const override;
};


#endif //TOMBEXCAVATOR_SPRITE_LOADER_HH
