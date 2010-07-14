#include "../sand.h"
#include "../Items.h"

#define PALACE_SPECIAL_REGEN	30000

PalaceClass::PalaceClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_Palace;
	

	structureSize.x = 3;
	structureSize.y = 3;

	deathOffX = deathOffY = NONE;

	maxHealth = PALACE_HITPOINTS;
	powerRequirement = 200;
	health = maxHealth;
	viewRange = PALACE_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building33].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	 /*if (getOwner()->isAI() && (settings.gameType == ORIGINAL))
		 specialTimer = 30000;
	 else*/
		 specialTimer = PALACE_SPECIAL_REGEN;
	
	interfaceContainer = new VBox();

	//interfaceContainer->setX(gameBarPos.x + 29);
	//interfaceContainer->setY(16 + 128 + 16);
	interfaceContainer->setX(gameBarPos.x + BUILDERBUTTONSMINX - 1);
	interfaceContainer->setY(BUILDERBUTTONSMINY - 1);
	//interfaceContainer->setWidth((screen->w - 16) - interfaceContainer->getX());
	interfaceContainer->setWidth(64);//(screen->w - 16) - interfaceContainer->getX());
	interfaceContainer->setHeight(48);
	//interfaceContainer->setHeight(interfaceContainer->getWidth());


	house = origHouse;

	switch (origHouse)
	{
		case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
			//palaceSpecial.setText("Fremen");
			picID = Picture_Fremen;
			break;
		case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
			//palaceSpecial.setText("Saboteur");
			picID = Picture_Saboteur;
			break;
		case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
			//palaceSpecial.setText("DeathHand");
			picID = Picture_DeathHand;
			break;
	}

	palaceSpecial.setHeight(48);
	palaceSpecial.setWidth(64);

	palaceSpecial.setSurface((SDL_Surface*)(dataFile[picID].dat), NULL);
	palaceSpecial.setIsProgress(true);
	palaceSpecial.setTotalProgress(PALACE_SPECIAL_REGEN);
	palaceSpecial.setProgress(0);
	palaceSpecial.setVisible(true);
	palaceSpecial.setCallBack(netDoSpecial, this);
	interfaceContainer->addChild(&palaceSpecial);

	enableSpecial(false);

	interfaceContainer->setVisible(true);
}
PalaceClass::~PalaceClass()
{
}

void PalaceClass::doSpecial()
{
	bool ok = false;
	switch (origHouse)
	{
		case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
			ok = callFremen();	
			break;
		case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
			ok = spawnSaboteur();
			break;
		case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
			ok = launchDeathhand();
			break;
	}

	if (ok)
	{
		enableSpecial(false);
		specialTimer = PALACE_SPECIAL_REGEN;
		
		palaceSpecial.setIsProgress(true);
		palaceSpecial.setTotalProgress(PALACE_SPECIAL_REGEN);
		palaceSpecial.setProgress(0);
	}
}

void PalaceClass::enableSpecial(bool status)
{
	palaceSpecial.setEnabled(status);
	
	//palaceSpecial.setVisible(status);

	if (status && (getOwner() == thisPlayer))
	{
		palaceSpecial.setText("READY");
		showMessage("Palace is ready");
	}
	else if(!status && (getOwner() == thisPlayer))
	{
		palaceSpecial.setText("");
	}
}

void PalaceClass::netDoSpecial(void* voidPalace)
{
	if (settings.playerType != CLIENT)
		((PalaceClass*)voidPalace)->doSpecial();
	else
		netClientPalaceSpecial(((PalaceClass*)voidPalace)->getOwner()->getPlayerNumber(), ((PalaceClass*)voidPalace)->getObjectID(), ((PalaceClass*)voidPalace)->getX(), ((PalaceClass*)voidPalace)->getY());

	if (settings.playerType == SERVER)
		netServerPalaceSpecial(((PalaceClass*)voidPalace)->getOwner()->getPlayerNumber(), ((PalaceClass*)voidPalace)->getObjectID(), ((PalaceClass*)voidPalace)->getX(), ((PalaceClass*)voidPalace)->getY(), ((PalaceClass*)voidPalace)->deathOffX, ((PalaceClass*)voidPalace)->deathOffY);
}

void PalaceClass::doSpecificStuff()
{
	if (specialTimer >= 0)
	{
		if (--specialTimer < 0)
		{
			if (settings.playerType != CLIENT)
				enableSpecial(true);
			if (settings.playerType == SERVER)
				netServerPalaceEnable(getOwner()->getPlayerNumber(), getObjectID(), getX(), getY());
			if (getOwner()->isAI())
				netDoSpecial(this);
		}
	}
	else if (getOwner()->isAI())
		netDoSpecial(this);

	if(house != origHouse)
	{
		switch (origHouse)
		{
		case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
			//palaceSpecial.setText("Fremen");
			picID = Picture_Fremen;
			break;
		case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
			//palaceSpecial.setText("Saboteur");
			picID = Picture_Saboteur;
			break;
		case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
			//palaceSpecial.setText("DeathHand");
			picID = Picture_DeathHand;
			break;
		}

		palaceSpecial.setSurface((SDL_Surface*)(dataFile[picID].dat), NULL);
	}


	if(getOwner() == thisPlayer)
	{
		palaceSpecial.setProgress(specialTimer);
	}
	else
	{
		palaceSpecial.setText("");
		palaceSpecial.setVisible(false);
	}
	//drawButton(palaceSpecial.getX(), palaceSpecial.getY());
}

void PalaceClass::setDeathhandOff(int newX, int newY)
{
	deathOffX = newX;
	deathOffY = newY;
}

void PalaceClass::setSpecialTimer(int time)
{
	if (time >= 0)
	{
		specialTimer = time;
		enableSpecial(time == 0);
	}
}

bool PalaceClass::callFremen()
{
	bool ok = true;
	if (settings.playerType != CLIENT)
	{
		int count, x, y;
		COORDTYPE coord;

		count = 0;
		do
		{
			x = getRandomInt(1, map->sizeX-2);
			y = getRandomInt(1, map->sizeY-2);
		} while (map->getCell(x-1, y-1)->hasAGroundObject()
				|| map->getCell(x, y-1)->hasAGroundObject()
				|| map->getCell(x+1, y-1)->hasAGroundObject()
				|| map->getCell(x-1, y)->hasAGroundObject()
				|| map->getCell(x, y)->hasAGroundObject()
				|| map->getCell(x+1, y)->hasAGroundObject()
				|| map->getCell(x-1, y+1)->hasAGroundObject()
				|| map->getCell(x, y+1)->hasAGroundObject()
				|| map->getCell(x+1, y+1)->hasAGroundObject()
				&& (count++ <= 1000));

		if (count < 1000)
		{
			int				i, j;
			InfantryClass	*fremen;

			for (count = 0; count < 20; count++)
			{
				if (getRandomInt(1, 10) <= 6)	//60% troopers
					fremen = (TrooperClass*)getOwner()->createUnit(Unit_Fremen);
				else
					fremen = (InfantryClass*)getOwner()->createUnit(Unit_Infantry);

				do
				{
					i = getRandomInt(-1, 1);
					j = getRandomInt(-1, 1);
				} while (!map->getCell(x + i, y + j)->infantryNotFull());

				coord.x = x + i;
				coord.y = y + j;
				fremen->deploy(&coord);
				if (settings.playerType == SERVER)
					netServerCreateObject(getOwner()->getPlayerNumber(), fremen->getItemID(), coord.x, coord.y, fremen->getAngle());
				fremen->setAttacking(true);
				fremen->setAttackMode(AGGRESSIVE);
				fremen->setRespondable(false);

				StructureClass* closestStructure = (StructureClass*)fremen->findClosestTargetStructure(fremen);
				if (closestStructure)
				{
					fremen->setGuardPoint(&closestStructure->getClosestPoint(fremen->getLocation()));
					fremen->setDestination(&closestStructure->getClosestPoint(fremen->getLocation()));
					if (settings.playerType == SERVER)
						fremen->netDoCommand(NULL, fremen->getDestination(), false);
				}
				else
				{
					UnitClass* closestUnit = (UnitClass*)fremen->findClosestTargetUnit(fremen);
					if (closestUnit)
					{
						fremen->setGuardPoint(closestUnit->getLocation());
						fremen->setDestination(closestUnit->getLocation());
						if (settings.playerType == SERVER)
							fremen->netDoCommand(NULL, fremen->getDestination(), false);
					}
				}
			}
		}
		else
		{
			ok = false;
			if (getOwner() == thisPlayer)
				showMessage("Unable to spawn Fremen");
		}
	}

	return ok;
}

bool PalaceClass::launchDeathhand()
{
	bool			ok = true;
	COORDTYPE		centrePoint,
					dest;

	if (getOwner()->isAI())
	{
		StructureClass* closestStructure = (StructureClass*)findClosestTargetStructure(this);
		if (closestStructure)
		{
			COORDTYPE temp = closestStructure->getClosestPoint(getLocation());
			setDestination(temp.x, temp.y);
		}
	}
	
	if (!getOwner()->isAI() && (getDestination()->x == NONE) && (getOwner() == thisPlayer))
	{
		ok = false;
		showMessage("Set Death Hand destination first");
	}
	else
	{
		if (settings.playerType != CLIENT)
		{
			//deathOffX = getRandomInt(-100, 100);
			//deathOffY = getRandomInt(-100, 100);
			deathOffX = getRandomInt(-150, 150);
			deathOffY = getRandomInt(-150, 150);
		}
		getCentrePoint(&centrePoint);
		dest.x = getDestination()->x * BLOCKSIZE + BLOCKSIZE/2 + deathOffX;
		dest.y = getDestination()->y * BLOCKSIZE + BLOCKSIZE/2 + deathOffY;

		bulletList->insertLast(new BulletClass(this, &centrePoint, &dest, Bullet_LargeRocket, false));
		soundPlayer->playSoundAt(Sound_rocket, getLocation());

		if (getOwner() == thisPlayer)
			showMessage("Missile launch detected");
	}

	return ok;
}

bool PalaceClass::spawnSaboteur()
{
	if (settings.playerType != CLIENT)
	{
		Saboteur*		saboteur = (Saboteur*)getOwner()->createUnit(Unit_Saboteur);
		COORDTYPE		spot = map->findDeploySpot(getLocation(), getDestination(), getStructureSize(), saboteur);

		saboteur->deploy(&spot);
		if (settings.playerType == SERVER)
			netServerCreateObject(getOwner()->getPlayerNumber(), saboteur->getItemID(), spot.x, spot.y, saboteur->getAngle());

		if (getOwner()->isAI())
		{
			saboteur->setAttacking(true);
			saboteur->setAttackMode(AGGRESSIVE);
		}
	}

	return true;
}

void PalaceClass::drawButton(int x, int y)
{
	char text[50];
	SDL_Surface	*textSurface;
	SDL_Rect drawLocation;
	SDL_Surface *graphic = (SDL_Surface*)(dataFile[picID].dat);

	drawLocation.x = x;
	drawLocation.y = y;
	drawLocation.w = graphic->w;
	drawLocation.h = graphic->h;
	SDL_BlitSurface(graphic, NULL, screen, &drawLocation);
	
	if (getPercentComplete() > 0)
	{
		int i, j;

		if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
		{
			for (i = 0; i < ((int)(getPercentComplete()*0.64)); i++)
			for (j = i % 2; j < 48; j+=2)
				putpixel(screen, x + i, y + j, COLOUR_BLACK);

			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);
		}
	}

	if (specialReady())
	{
		sprintf(text, "READY");
		textSurface = TTF_RenderText_Solid(font[10], text, palette->colors[COLOUR_WHITE]);
		drawLocation.x = x + (graphic->w - textSurface->w)/2;
		drawLocation.y = y + (graphic->h - textSurface->h)/2;
		drawLocation.w = textSurface->w;
		drawLocation.h = textSurface->h;
		SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);
		SDL_FreeSurface(textSurface);
	}

	showMessage("button drawn");
}

double PalaceClass::getPercentComplete()
{
	return specialTimer*100.0/PALACE_SPECIAL_REGEN;
}

