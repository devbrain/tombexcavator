//
// Created by igor on 28/03/2021.
//

#include <tomb-excavator/games/common/vfs/virtual_fs.hh>
#include <tomb-excavator/games/westwood/westwood_data_loader.hh>
#include "dune_2_pak_reader.hh"

using namespace games::common;
using namespace games::westwood;

class dune2_data_loader : public games::westwood::root_directory_loader
{
public:
    dune2_data_loader();
private:
    std::unique_ptr<physical_data_loader> create_pak_loader(const std::string& pak_name) override;
};

dune2_data_loader::dune2_data_loader()
: games::westwood::root_directory_loader({"dune.pak"})
{

}
// --------------------------------------------------------------------------------------------
std::unique_ptr<physical_data_loader> dune2_data_loader::create_pak_loader(const std::string& pak_name)
{
    return std::make_unique<dune2_pak_reader>(pak_name);
}
// --------------------------------------------------------------------------------------------
class dune2 : public virtual_fs
{
public:
    dune2()
            : virtual_fs("Dune 2",
                         {new dune2_data_loader()})
    {

    }
};

// ----------------------------------------------------------------------------------------
REGISTER_PROVIDER_FS(dune2)
