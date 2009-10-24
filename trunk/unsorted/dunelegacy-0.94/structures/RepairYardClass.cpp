#include "../sand.h"
#include "../Items.h"

RepairYardClass::RepairYardClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_RepairYard;
	picID = Picture_RepairYard;

	structureSize.x = 3;
	structureSize.y = 2;

	bookings = 0;
	repairing = false;
	repairUnit = NULL;

	maxHealth = REPAIRYARD_HITPOINTS;
	powerRequirement = 30;
	health = maxHealth;
	viewRange = REPAIRYARD_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building23].dat);
	imageW = graphic->w/3;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	animFrames = 3;
	animFrame = 0;
	isAnimating = 0;

	interfaceContainer = new VBox();
	interfaceContainer->setX(gameBarPos.x + BUILDERBUTTONSMINX - 1);
	interfaceContainer->setY(BUILDERBUTTONSMINY - 1);
	interfaceContainer->setWidth(64);
	interfaceContainer->setHeight(48);

	SDL_Surface* pic_graphic = (SDL_Surface*)(dataFile[picID].dat);
	repairProgress.setHeight(pic_graphic->h);
	repairProgress.setWidth(pic_graphic->w);

	repairProgress.setSurface(pic_graphic, NULL);
	repairProgress.setIsProgress(false);
	repairProgress.setVisible(true);

	interfaceContainer->addChild(&repairProgress);
	interfaceContainer->setVisible(true);
}

RepairYardClass::~RepairYardClass()
{
}

void RepairYardClass::deployRepairUnit()
{
	unBook();
	repairing = false;
	animFrame = 0;
	isAnimating = 0;
	repairUnit->deploy(&map->findDeploySpot(&location, &destination, &structureSize, repairUnit));
	
	if(getOwner() == thisPlayer)
	{
		switch (getOwner()->getHouse())
		{
			case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
				soundPlayer->playVOSoundDual(SoundVO_Avehicle, SoundVO_Arepair);
				break;
			case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
				soundPlayer->playVOSoundDual(SoundVO_Ovehicle, SoundVO_Orepair);
				break;
			case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
				soundPlayer->playVOSoundDual(SoundVO_Hvehicle, SoundVO_Hrepair);
				break;
			default:
				break;
		}

		repairProgress.setSurface((SDL_Surface*)(dataFile[picID].dat), NULL);
		repairProgress.setIsProgress(false);
		repairProgress.setTotalProgress(0);
		repairProgress.setProgress(0);

	}
}

void RepairYardClass::doSpecificStuff()
{
/*	if (selected)
	{
		char msg[50];
		sprintf(msg, "num bookings: %d", bookings);
		showMessage(msg);
	}*/
	isAnimating = repairing;

	if (repairing)
	{
		int newPicID = Picture_RepairYard;


		newPicID = resolveItemPicture(repairUnit->getItemID());

		if(getOwner() == thisPlayer && newPicID != -1)
		{
			repairProgress.setSurface((SDL_Surface*)(dataFile[newPicID].dat), NULL);
			repairProgress.setIsProgress(true);
			repairProgress.setTotalProgress(repairUnit->getMaxHealth());
			repairProgress.setProgress(repairUnit->getHealth());
		}

		if (repairUnit->getHealth()*100/repairUnit->getMaxHealth() < 100)
		{
			if (owner->takeCredits(UNIT_REPAIRCOST) > 0)
			{
				repairUnit->addHealth();
				
			}
		}
		else
		{
			deployRepairUnit();
		}

		
	}
}

