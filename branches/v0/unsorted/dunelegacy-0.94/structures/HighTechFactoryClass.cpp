#include "../sand.h"
#include "../Items.h"

HighTechFactoryClass::HighTechFactoryClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_HighTechFactory;

	structureSize.x = 3;
	structureSize.y = 2;

	builder = true;

	maxHealth = HIGHTECHFACTORY_HITPOINTS;
	powerRequirement = 35;
	health = maxHealth;
	viewRange = HIGHTECHFACTORY_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building23].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle


}

HighTechFactoryClass::~HighTechFactoryClass()
{
}

void HighTechFactoryClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	insertItem(Unit_Carryall);
	if (settings.techLevel >= 7 && (origHouse != HOUSE_HARKONNEN || origHouse != HOUSE_SARDAUKAR))
		insertItem(Unit_Ornithopter);

	checkMinMaxSelection();
}
void HighTechFactoryClass::doSpecificStuff()
{
	buildUpdate();
}
