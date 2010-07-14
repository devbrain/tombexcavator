#include "../sand.h"
#include "../Items.h"


extern SDL_Surface *fogSurf;

StructureClass::StructureClass(PlayerClass* newOwner) : ObjectClass(newOwner)
,fogged(false)
{
	itemID = Unknown;


	aStructure = true;
	powerRequirement = 10;
	structureSize.x = 0;
	structureSize.y = 0;

	builder = false;
	justPlaced = false;
	repairing = false;

	armour = 0;
	drawnAngle = 0;
	angle = drawnAngle;

	destination.x = NONE;
	destination.y = NONE;

	placeGraphic = NULL;
	deathGraphic = NULL;
	numDeathFrames = 5;

	origHouse = newOwner->getHouse();

	if(thisPlayer == newOwner)
	{
		interfaceContainer = new Container();
		interfaceContainer->setX(gameBarPos.x + 29);//gameBarPos.x + 29);
		interfaceContainer->setY(145);//was 16 + 128 + 16
		interfaceContainer->setWidth(100);//(screen->w - 16) - interfaceContainer->getX());
		interfaceContainer->setHeight(screen->h - interfaceContainer->getY());


		SDL_Surface *RepairUp = copySurface((SDL_Surface*)(dataFile[UI_Repair].dat));
		SDL_Surface *RepairDown = copySurface((SDL_Surface*)(dataFile[UI_Repair_Pressed].dat));

		repairButton.setSurface(RepairUp, NULL);
		repairButton.setPressedSurface(RepairDown, NULL);

		repairButton.setX(interfaceContainer->getX());
		repairButton.setY(interfaceContainer->getY());
		repairButton.setHeight(RepairUp->h);
		repairButton.setWidth(RepairUp->w);
	
		repairButton.setCallBack(repairCallback, this);
		repairButton.setVisible(true);

		interfaceContainer->setVisible(true);
		interfaceContainer->addChild(&repairButton);
	}
}

StructureClass::~StructureClass()
{

	if (destroyed)
	{
		for(int i = 0; i < structureSize.x; i++)
			delete[] deathGraphic[i];

		delete[] deathGraphic;

	}



	if (map->cellExists(&location))

		map->getCell(&location)->unassignDeadObject(this);

	getOwner()->unsapPower(powerRequirement);

}


void StructureClass::assignToMap(COORDTYPE* pos)

{

	COORDTYPE temp;

	for (int i = getX(); i < getX() + structureSize.x; i++)

		for (int j = getY(); j < getY() + structureSize.y; j++)

		{

			temp.x = i; temp.y = j;

			if (map->cellExists(i, j))

			{

				map->cell[i][j].assignNonInfantryGroundObject(this);

				if ((itemID != Structure_Wall) && (itemID != Structure_ConstructionYard) && !map->cell[i][j].isConcrete() && settings.concreteRequired && (gameState != START))

					health -= (int)(0.5*(double)maxHealth/((double)(structureSize.x*structureSize.y)));

				map->cell[i][j].setType(Terrain_Rock);

				map->cell[i][j].setOwner(getOwner()->getPlayerNumber());

				map->viewMap(getOwner()->getTeam(), &temp, getViewRange());



				setVisible(VIS_ALL, true);

				setRespondable(true);

			}

		}

}


void StructureClass::blitToScreen()
{
	if (!destroyed)
	{
		drawGraphic();

		if (selected)
			selectionBlitList->insertLast(this);
	}

	else

	{

		int i, j;

		SDL_Rect dest, source;



		dest.w = source.w = imageW;

		dest.h = source.h = imageH;

		source.x = deathFrame*imageW;

		source.y = 0;

		
		for(i = 0; i < structureSize.x; i++)
		for(j = 0; j < structureSize.y; j++)

		{

			dest.x = getDrawnX() + i*BLOCKSIZE;

			dest.y = getDrawnY() + j*BLOCKSIZE;

			SDL_BlitSurface(deathGraphic[i][j], &source, screen, &dest);

		}

	}
}

void StructureClass::drawGraphic()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	source.x = imageW * drawnAngle;

	if ((imageH != graphic->h) && (++counter > FLAGMOVETIMER))
	{
		source.y = imageH;
		if (counter > 2*FLAGMOVETIMER)
			counter = 0;
	}
	else
		source.y = 0;

	//Animation for buildings
	if ((imageW != graphic->w) && (++animcounter > ANIMMOVETIMER) && isAnimating)
	{	
		animFrame++;

		if(animFrame == animFrames)
			animFrame = 0;
		
		animcounter = 0;
	}

	source.x = imageW*animFrame;

	if(fogged)
	{
		SDL_BlitSurface(graphic, &lastVisible, screen, &dest);
		SDL_BlitSurface(fogSurf, &lastVisible, screen, &dest);
	}
	else
	{
	lastVisible = source;
	if (justPlaced && placeGraphic && ((frameTimer / 50)%2 == 1))
		SDL_BlitSurface(placeGraphic, NULL, screen, &dest);
	else
	{
		SDL_BlitSurface(graphic, &source, screen, &dest);
		if (justPlaced && (frameTimer <= 0))
			justPlaced = false;
	}
	}
}

void StructureClass::drawSelectRect()
{
	SDL_Rect dest;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = imageW;
	dest.h = imageH;

	//now draw the selection box thing, with parts at all corners of structure
	if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
	{
		putpixel(screen, dest.x, dest.y, COLOUR_WHITE);	//top left bit
		putpixel(screen, dest.x+1, dest.y, COLOUR_WHITE);
		putpixel(screen, dest.x, dest.y+1, COLOUR_WHITE);

		putpixel(screen, dest.x-1 + imageW, dest.y, COLOUR_WHITE);	//top right bit
		putpixel(screen, dest.x-2 + imageW, dest.y, COLOUR_WHITE);
		putpixel(screen, dest.x-1 + imageW, dest.y+1, COLOUR_WHITE);

		putpixel(screen, dest.x, dest.y-1 + imageH, COLOUR_WHITE);	//bottom left bit
		putpixel(screen, dest.x+1, dest.y-1 + imageH, COLOUR_WHITE);
		putpixel(screen, dest.x, dest.y-2 + imageH, COLOUR_WHITE);

		putpixel(screen, dest.x-1 + imageW, dest.y-1 + imageH, COLOUR_WHITE);	//bottom right bit
		putpixel(screen, dest.x-2 + imageW, dest.y-1 + imageH, COLOUR_WHITE);
		putpixel(screen, dest.x-1 + imageW, dest.y-2 + imageH, COLOUR_WHITE);

		if (SDL_MUSTLOCK(screen))
			SDL_UnlockSurface(screen);
	}

	drawhline(screen, dest.x, dest.y-2, dest.x + ((int)(((double)health/(double)maxHealth)*(BLOCKSIZE*structureSize.x - 1))), getHealthColour());
}



void StructureClass::getCentrePoint(COORDTYPE* centrePoint)

{

	centrePoint->x = round(realX + structureSize.x*BLOCKSIZE/2);

	centrePoint->y = round(realY + structureSize.y*BLOCKSIZE/2);

}



void StructureClass::getClosestCentrePoint(COORDTYPE* centrePoint, COORDTYPE* objectLocation)

{

	*centrePoint = getClosestPoint(objectLocation);

	centrePoint->x = centrePoint->x*BLOCKSIZE + BLOCKSIZE/2;

	centrePoint->y = centrePoint->y*BLOCKSIZE + BLOCKSIZE/2;

}



int StructureClass::getDrawnX()

{

	return ((int)(realX - xOffset) - dborder->minX);

}



int StructureClass::getDrawnY()

{

	return ((int)(realY - yOffset) - dborder->minY);

}

void StructureClass::handleActionCommand(int xPos, int yPos)
{
	if ((xPos < location.x) || (xPos >= (location.x + structureSize.x)) || (yPos < location.y) || (yPos >= (location.y + structureSize.y)))

		setDestination(xPos, yPos);
	else

		setDestination(NONE, NONE);

	netDoCommand(NULL, &destination, false);
}

void StructureClass::setDestination(int newX, int newY)
{
	if (map->cellExists(newX, newY) || ((newX == NONE) && (newY == NONE)))
	{
		destination.x = newX;
		destination.y = newY;
	}
}

void StructureClass::setJustPlaced()
{
	if (!destroyed)
	{
		justPlaced = true;
		frameTimer = 300;
	}
}

void StructureClass::update()
{
	if(itemID == 223)
	{
		fprintf(stderr, "we are in update for turretPlacing thing %d\n", itemID);
	}

	if (!destroyed)
	{
		//update map
		if(getRandomInt(0,40) == 0)
		map->viewMap(owner->getTeam(), &location, viewRange);				
		doSpecificStuff();

		if (health <= 0)
			netDestroy();
		else if (repairing)
		{
			if (owner->getAmountOfCredits() >= 5)
			{
				owner->takeCredits(0.05);
				health += 0.5;
				if (health >= maxHealth)
				{
					health = maxHealth;
					repairing = false;
					repairButton.setVisible(false);
				}
			}
			else
			{
				repairButton.setVisible(false);
				repairing = false;
			}
		}
		else if (owner->isAI() && (((double)health/(double)maxHealth) < 0.75))

			repair();

		if(health < maxHealth)
			repairButton.setVisible(true);
	}
	if (frameTimer > 0)
	{
		if (destroyed)
		{	//death frame has started
			if (frameTimer == 1)
			{
				deathFrame++;

				if (deathFrame < numDeathFrames)

					frameTimer = frameTime;
			}
		}
		frameTimer--;
	}
}
void StructureClass::destroy()
{
	if (!destroyed)
	{
		int	i, j, num;

		map->removeObjectFromMap(this);	//no map point will reference now
		owner->decrementStructures(itemID, &location);

		SDL_FreeSurface(graphic);
		destroyed = true;

		deathGraphic = new SDL_Surface**[structureSize.x];
		for(i = 0; i < structureSize.x; i++)
			deathGraphic[i] = new SDL_Surface*[structureSize.y];

		for(i = 0; i < structureSize.x; i++)
		for(j = 0; j < structureSize.y; j++)
		{
			num = getRandomInt(0, 1) ? Object_LargeExplosion1 : Object_LargeExplosion2;	//50% chance of either
			deathGraphic[i][j] = (SDL_Surface*)dataFile[num].dat;
		}
		imageW = deathGraphic[0][0]->w/numDeathFrames;
		imageH = deathGraphic[0][0]->h;
		xOffset = (imageW - BLOCKSIZE)/2;		    //this is where it actually draws the graphic
		yOffset = (imageH - BLOCKSIZE)/2;		    //cause it draws at top left, not middle
		frameTimer = frameTime;

		deathFrame = 0;


		if (isVisible(thisPlayer->getTeam()))
			soundPlayer->playSoundAt(Sound_structureExplosion, &location);
		removeFromSelectionLists();



		if (map->cellExists(&location))

			map->getCell(&location)->assignDeadObject(this);
	}
}

void StructureClass::repair()
{
	repairing = true;
}



void StructureClass::repairCallback(void *val)

{

	((StructureClass *)val)->repair();

}



COORDTYPE StructureClass::getClosestPoint(COORDTYPE* objectLocation)

{

	COORDTYPE closestPoint;



	//find the closest cell of a structure from a location

	if (objectLocation->x <= location.x)	//if we are left of the structure

		closestPoint.x = location.x;	//set destination, left most point

	else if (objectLocation->x >= (location.x + structureSize.x-1))	//vica versa

		closestPoint.x = location.x + structureSize.x-1;

	else

		closestPoint.x = objectLocation->x;	//we are above or below at least on cell of the structure, closest path is straight



	//same deal but with y

	if (objectLocation->y <= location.y)

		closestPoint.y = location.y;

	else if (objectLocation->y >= (location.y + structureSize.y-1))

		closestPoint.y = location.y + structureSize.y-1;

	else

		closestPoint.y = objectLocation->y;



	return closestPoint;

}

