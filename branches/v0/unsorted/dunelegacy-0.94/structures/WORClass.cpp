#include "../sand.h"
#include "../Items.h"

WORClass::WORClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_WOR;

	structureSize.x = 2;
	structureSize.y = 2;

	builder = true;

	maxHealth = WOR_HITPOINTS;
	powerRequirement = 20;
	health = maxHealth;
	viewRange = WOR_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

WORClass::~WORClass()
{
}

void WORClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

//	insertItem(Unit_Infantry);
	switch(owner->getHouse())
	{
		case(HOUSE_ATREIDES): case(HOUSE_ORDOS): case(HOUSE_HARKONNEN): case(HOUSE_MERCENARY):
			insertItem(Unit_Trooper);
			break;
		case(HOUSE_FREMEN):
			insertItem(Unit_Fremen);
			break;
		 case(HOUSE_SARDAUKAR):
			insertItem(Unit_Sardaukar);
			break;
		default:
			break;
	}
	


	checkMinMaxSelection();
}
void WORClass::doSpecificStuff()
{
	buildUpdate();
}

