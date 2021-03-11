//
// Created by igor on 22/02/2021.
//

#include <vector>

#include <tomb-excavator/games/common/flat_provider/flat_provider.hh>
#include <tomb-excavator/games/common/loaders/picture_loader.hh>
#include <tomb-excavator/games/apogee/prographx_sprite_loader.hh>

class ccfs : public games::common::flat_provider
{
public:
    ccfs();
};

// ----------------------------------------------------------------------------------------
ccfs::ccfs()
        : games::common::flat_provider("Crystal Caves",
                                       {
                                               new apogee::prographx_sprite_loader("sprites", "cc1.gfx", 0, 0),
                                               new apogee::prographx_sprite_loader("menu-tiles", "cc1-f1.mni", 0, 0),
                                               new games::common::picture_loader("apogee", "cc1.apg"),
                                               new games::common::picture_loader("title", "cc1.ttl"),
                                               new games::common::picture_loader("end", "cc1.end"),
                                               new games::common::picture_loader("credits", "cc1.cdt")
                                       }
)
{

}
// ----------------------------------------------------------------------------------------
REGISTER_PROVIDER_FS(ccfs)
