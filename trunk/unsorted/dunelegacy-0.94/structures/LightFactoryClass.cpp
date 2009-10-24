#include "../sand.h"
#include "../Items.h"

LightFactoryClass::LightFactoryClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_LightFactory;

	structureSize.x = 2;
	structureSize.y = 2;

	builder = true;

	maxHealth = LIGHTFACTORY_HITPOINTS;
	powerRequirement = 20;
	health = maxHealth;
	viewRange = LIGHTFACTORY_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	upgradeLevels = 1;
	curUpgradeLev = 0;
	upgradeCost = 500;
	allowedToUpgrade = false;
}

LightFactoryClass::~LightFactoryClass()
{
}

void LightFactoryClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();
	if(settings.techLevel>=3)
		allowedToUpgrade = true;

	switch(origHouse) 
	{
	case HOUSE_ORDOS:
		insertItem(Unit_Raider);
		if(curUpgradeLev)
		  insertItem(Unit_Quad);
		break;
	case HOUSE_ATREIDES:
		insertItem(Unit_Trike);
		if(curUpgradeLev)
		  insertItem(Unit_Quad);
		break;
	case HOUSE_HARKONNEN:
		insertItem(Unit_Quad);
		break;
	default:
		insertItem(Unit_Quad);
		insertItem(Unit_Trike);
		break;	
	};
	checkMinMaxSelection();
}
void LightFactoryClass::doSpecificStuff()
{
	buildUpdate();
}

