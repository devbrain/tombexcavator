//
// Created by igor on 28/03/2021.
//

#ifndef TOMBEXCAVATOR_DUNE_2_PAK_READER_HH
#define TOMBEXCAVATOR_DUNE_2_PAK_READER_HH

#include <tomb-excavator/games/westwood/westwood_pak_loader.hh>

class dune2_pak_reader : public games::westwood::pak_loader
{
public:
    explicit dune2_pak_reader(std::string phys_name);
private:
    std::optional<file_data_t> get_file_data(const std::string& name) override;
};


#endif //TOMBEXCAVATOR_DUNE_2_PAK_READER_HH
