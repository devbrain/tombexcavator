#include "../sand.h"
#include "../Items.h"

RefineryClass::RefineryClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_Refinery;

	structureSize.x = 3;
	structureSize.y = 2;

	bookings = 0;
	firstRun = 1;
	extractingSpice = false;
	harvester = NULL;

	maxHealth = REFINERY_HITPOINTS;
	powerRequirement = 30;
	health = maxHealth;
	viewRange = REFINERY_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building23].dat);
	imageW = graphic->w/4;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	animFrames = 3;//Actually 4 frames, but 4th is harvester
	animFrame = 0;
	isAnimating = 0;
}

RefineryClass::~RefineryClass()
{
	if (extractingSpice)
		harvester->netDestroy();
}

void RefineryClass::assignHarvester(HarvesterClass* newHarvester)
{
	if (owner == thisPlayer)
		owner = thisPlayer;
 	harvester = newHarvester;
	extractingSpice = true;
	drawnAngle = 1;
}

void RefineryClass::deployHarvester()
{
	unBook();
	drawnAngle = 0;
	extractingSpice = false;
	isAnimating = 0;
	animFrame = 0;

	if(firstRun)
	{
		if (getOwner() == thisPlayer)
		{
			switch (getOwner()->getHouse())
			{
			case(HOUSE_ATREIDES): 
				soundPlayer->playVOSoundThree(SoundVO_Aatre, SoundVO_Aharvest, SoundVO_Adeploy);
				break;
			case(HOUSE_FREMEN):
				soundPlayer->playVOSoundThree(SoundVO_Afremen, SoundVO_Aharvest, SoundVO_Adeploy);
				break;
			case(HOUSE_ORDOS): 
				soundPlayer->playVOSoundThree(SoundVO_Oordos, SoundVO_Oharvest, SoundVO_Odeploy);
				break;
			case(HOUSE_MERCENARY):
				soundPlayer->playVOSoundThree(SoundVO_Omerc, SoundVO_Oharvest, SoundVO_Odeploy);
				break;
			case(HOUSE_HARKONNEN): 
				soundPlayer->playVOSoundThree(SoundVO_Hhark, SoundVO_Hharvest, SoundVO_Hdeploy);
				break;
			case(HOUSE_SARDAUKAR):
				soundPlayer->playVOSoundThree(SoundVO_Hsard, SoundVO_Hharvest, SoundVO_Hdeploy);
				break;
			default:
				break;
			}
		}
	}

	firstRun = 0;

	harvester->deploy(&map->findDeploySpot(&location, &destination, &structureSize, harvester));
}

void RefineryClass::startAnimate()
{
	isAnimating = 1;
}

void RefineryClass::doSpecificStuff()
{
/*	if (selected)
	{
		char msg[50];
		sprintf(msg, "num bookings: %d", bookings);
		showMessage(msg);
	}*/

	if (extractingSpice)
	{
		if (harvester->getAmountOfSpice() > 0.0)
		{
			owner->addCredits(harvester->extractSpice());
			animFrame = 3;
			isAnimating = 0;
		}
		else
			deployHarvester();
	}
	/*
	else if(harvester->isReturning())
	{
		isAnimating = 1;
	}*/
}

