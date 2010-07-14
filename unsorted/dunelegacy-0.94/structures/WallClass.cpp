#include "../sand.h"
#include "../Items.h"

WallClass::WallClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_Wall;

	structureSize.x = 1;
	structureSize.y = 1;

	drawnAngle = Structure_w8;
	maxHealth = WALL_HITPOINTS;
	powerRequirement = 0;
	health = maxHealth;
	viewRange = WALL_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/11;
	imageH = graphic->h;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

WallClass::~WallClass()
{
}
void WallClass::checkSelf()
{
}
void WallClass::doSpecificStuff()
{
}

