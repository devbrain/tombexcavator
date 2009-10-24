#include "../sand.h"
#include "../Items.h"

ConstructionYardClass::ConstructionYardClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_ConstructionYard;

	structureSize.x = 2;
	structureSize.y = 2;

	builder = true;

	maxHealth = 1500;
	powerRequirement = 0;
	health = maxHealth;
	viewRange = CONSTRUCTIONYARD_VIEWRANGE;
			
	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_ConstructionYardPlace].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	upgradeLevels = 1;
	curUpgradeLev = 0;
	upgradeCost = 500;
}

ConstructionYardClass::~ConstructionYardClass()
{
}

void ConstructionYardClass::checkSelectionList()
{	//needed so selection list will have things in the right order when changed
	BuilderClass::checkSelectionList();
	
	insertItem(Structure_Slab1);
	if (settings.techLevel >= 4 && curUpgradeLev >= 1)
		insertItem(Structure_Slab4);
	insertItem(Structure_Wall);
	insertItem(Structure_WindTrap);
	if (owner->hasWindTrap() || ( ((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
	{
		insertItem(Structure_Refinery);
		if ((owner->hasRefinery() && (settings.techLevel >= 2)) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && getOwner()->isAI()))
		{
			insertItem(Structure_Silo);
			insertItem(Structure_Radar);
			/*no radar needed for StarPort and LightFactory*/
			if (settings.techLevel >= 6)
				insertItem(Structure_StarPort);	
			if (settings.techLevel >= 3)
				insertItem(Structure_LightFactory);
			
		}
		
		
		if (owner->hasRadar() || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
		{
			if ((settings.techLevel >= 5) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
				insertItem(Structure_GunTurret);
			if ((settings.techLevel >= 6) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
				insertItem(Structure_RocketTurret);

			switch (getOwner()->getHouse())
			{
				case(HOUSE_ATREIDES): 
					insertItem(Structure_Barracks);	
					break;
				case(HOUSE_FREMEN):
					insertItem(Structure_Barracks);
					insertItem(Structure_WOR);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					insertItem(Structure_Barracks);
					if (settings.techLevel >= 8)
						insertItem(Structure_WOR);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					insertItem(Structure_WOR);
					break;
			}

			if (owner->hasLightFactory() || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && getOwner()->isAI()))
			{
				if ((settings.techLevel >= 4) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
				{
					insertItem(Structure_HeavyFactory);
					
				}

				if ((settings.techLevel >= 5) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
				{
					insertItem(Structure_HighTechFactory);
				}
			}

			if(owner->hasHeavyFactory())
			{
				if ((settings.techLevel >= 5) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH))  && getOwner()->isAI()))
				{
					
					insertItem(Structure_RepairYard);
				}
			}

			if ((owner->hasStarPort() && (settings.techLevel >= 7)) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && getOwner()->isAI()))
			{
				insertItem(Structure_IX);

				if ((owner->hasIX() && (settings.techLevel >= 8)) || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && getOwner()->isAI()))
					insertItem(Structure_Palace);
			}
		}
		else if (settings.techLevel >= 2)
		{
			switch (getOwner()->getHouse())
			{
				case(HOUSE_ATREIDES): 
					insertItem(Structure_Barracks);	
					break;
				case(HOUSE_FREMEN):
					insertItem(Structure_Barracks);
					insertItem(Structure_WOR);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					insertItem(Structure_Barracks);
					if (settings.techLevel >= 8)
						insertItem(Structure_WOR);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					insertItem(Structure_WOR);
					break;
			}
		
		}
	}

	checkMinMaxSelection();
}
void ConstructionYardClass::doSpecificStuff()
{
	buildUpdate();
}

