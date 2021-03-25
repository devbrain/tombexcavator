//
// Created by igor on 22/02/2021.
//

#include <vector>

#include <tomb-excavator/games/common/vfs/virtual_fs.hh>
#include <tomb-excavator/games/common/loaders/picture_loader.hh>
#include <tomb-excavator/games/apogee/prographx_sprite_loader.hh>
#include "ccmap.hh"

using namespace apogee;
using namespace games::common;

class ccfs : public virtual_fs
{
public:
    ccfs()
            : virtual_fs("Crystal Caves",
                         {
                                    new prographx_sprite_loader("sprites", "cc1.gfx", 0, 0),
                                    new prographx_sprite_loader("menu-tiles", "cc1-f1.mni", 0, 45),
                                    new prographx_sprite_loader("fonts-spl", "cc1-spl.mni", 0, 45),
                                    new picture_loader("apogee", "cc1.apg"),
                                    new picture_loader("title", "cc1.ttl"),
                                    new picture_loader("end", "cc1.end"),
                                    new picture_loader("credits", "cc1.cdt"),
                                    new ccmap("cc1.exe")
                            }
    )
    {

    }
};

// ----------------------------------------------------------------------------------------
REGISTER_PROVIDER_FS(ccfs)
