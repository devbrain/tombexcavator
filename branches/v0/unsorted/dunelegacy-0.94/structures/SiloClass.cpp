#include "../sand.h"
#include "../Items.h"

SiloClass::SiloClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_Silo;

	structureSize.x = 2;
	structureSize.y = 2;

	maxHealth = SILO_HITPOINTS;
	powerRequirement = 5;
	health = maxHealth;
	viewRange = SILO_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

SiloClass::~SiloClass()
{
}
void SiloClass::doSpecificStuff()
{

}

