#include "../sand.h"
#include "../Items.h"

HeavyFactoryClass::HeavyFactoryClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_HeavyFactory;

	structureSize.x = 3;
	structureSize.y = 2;

	builder = true;

	maxHealth = HEAVYFACTORY_HITPOINTS;
	powerRequirement = 35;
	health = maxHealth;
	viewRange = HEAVYFACTORY_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building23].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	upgradeLevels = 2;
	curUpgradeLev = 0;
	upgradeCost = 500;
}

HeavyFactoryClass::~HeavyFactoryClass()
{
}

void HeavyFactoryClass::buildRandom()
{
	BuildItemClass* item;

	do
		item = reinterpret_cast<BuildItemClass*>(selectionList->getRandomElement());
	while ((item->getItemID() == Unit_Harvester) || (item->getItemID() == Unit_MCV));
	//netDoBuildCommand(false, getRandomInt(0, getNumSelections()-1));
	buildType(item->getItemID());
}

void HeavyFactoryClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();


	if (getOwner()->hasIX())
		switch(origHouse)
		{
		case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
			insertItem(Unit_SonicTank);
			break;
		case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
			insertItem(Unit_Deviator);
			break;
		case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
			insertItem(Unit_Devastator);
			break;
		default:
			break;
	}

	if (settings.techLevel >= 6 && curUpgradeLev >= 2)
		insertItem(Unit_SiegeTank);
	if (settings.techLevel >= 5 && (owner->getHouse() != HOUSE_ORDOS && owner->getHouse() != HOUSE_MERCENARY))
		insertItem(Unit_Launcher);
	insertItem(Unit_Tank);
	insertItem(Unit_Harvester);

	if(curUpgradeLev > 0)
		insertItem(Unit_MCV);

	checkMinMaxSelection();
}
void HeavyFactoryClass::doSpecificStuff()
{


	buildUpdate();
}

