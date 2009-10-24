#include "../sand.h"
#include "../Items.h"

IXClass::IXClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_IX;

	structureSize.x = 2;
	structureSize.y = 2;

	maxHealth = IX_HITPOINTS;
	powerRequirement = 40;
	health = maxHealth;
	viewRange = IX_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

IXClass::~IXClass()
{
}

void IXClass::doSpecificStuff()
{

}

