#include "../sand.h"

#include "../Items.h"

/*how often is the same sandframe redrawn*/
#define HARVESTERDELAY 30


COORDTYPE	harvesterSandOffset[] =

{

	{-10, 5},

	{-3, 9},

	{4, 10},

	{11, 9},

	{18, 5},

	{14, -2},

	{4, -5},

	{-5, -2}

};



HarvesterClass::HarvesterClass(PlayerClass* newOwner) : TrackedUnit(newOwner)

{

	baseID = itemID = Unit_Harvester;



	armour = 12;

	speed = 0.3;

	maxHealth = 800;

	health = maxHealth;

	radius = 8;

	viewRange = HARVESTER_VIEWRANGE;

	weaponRange = HARVESTER_WEAPONRANGE;



	attackMode = STANDGROUND;

	canAttackStuff = false;

	harvesting = false;

	returningToRefinery = false;

	spice = 0;



	graphic = (SDL_Surface*)(dataFile[baseID].dat);

	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());

	imageW = graphic->w/NUM_ANGLES;

	imageH = graphic->h;

	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place

	yOffset = (imageH - BLOCKSIZE)/2;

}



HarvesterClass::~HarvesterClass()

{



}



void HarvesterClass::blitToScreen()

{

	SDL_Rect dest, source;

	dest.x = getDrawnX();

	dest.y = getDrawnY();

	dest.w = source.w = imageW;

	dest.h = source.h = imageH;

	source.x = drawnAngle*imageW;

	source.y = 0;



	if (!destroyed)

	{

		SDL_BlitSurface(graphic, &source, screen, &dest);

		if (harvesting && (map->cell[location.x][location.y].hasSpice()))	//if sand should be shown - extracting spice

		{

			/*using counter to avoid changing the picture to often*/
			static int counter = 0;
			static int lastY;
			SDL_Surface* sand = (SDL_Surface*)(dataFile[Unit_HarvesterSand].dat);

			source.x = drawnAngle*sand->w/8;

			if (counter++ >= HARVESTERDELAY)
			{
			counter = 0;
			source.y = sand->h/3*getRandomInt(0, LASTSANDFRAME);
			lastY = source.y;
			}
			else
			{
			  source.y = lastY;
			};

			dest.w = source.w = sand->w/8;

			dest.h = source.h = sand->h/3;

			dest.x = getDrawnX() + harvesterSandOffset[drawnAngle].x;

			dest.y = getDrawnY() + harvesterSandOffset[drawnAngle].y;

			

			SDL_BlitSurface(sand, &source, screen, &dest);

		}

		if (badlyDamaged)

			drawSmoke(round(realX - dborder->minX), round(realY - dborder->minY));

		if (selected)

			selectionBlitList->insertLast(this);

	}

	else

	{

		source.x = deathFrame*imageW;

		source.y = 0;

		SDL_BlitSurface(graphic, &source, screen, &dest);

	}

}



void HarvesterClass::checkPos()

{

	TrackedUnit::checkPos();

	if (active)

	{

		if (returningToRefinery)

		{

			if (target && (target->getItemID() == Structure_Refinery))

			{	//find a refinery to return to

				if (blockDistance(&location, &target->getClosestPoint(&location)) <= 1.5)

				{

					awaitingPickup = false;

					if (((RefineryClass*)target)->isFree())

						setReturned();

				}

				else if (!awaitingPickup)

					requestCarryall();

			}

			else if (!structureList->isEmpty())

			{

				int	leastNumBookings = 1000000,//huge amount so refinery couldn't possibly compete with any refinery num bookings

					tempNumBookings = 0;



				double	closestLeastBookedRefineryDistance = 1000000,//same deal

						tempDistance = 0;



				RefineryClass	*bestRefinery = NULL,

								*tempRefinery;

				StructureClass* tempStructure;



				structureList->reset();

				while(structureList->currentNotNull())

				{

					tempStructure = (StructureClass*)structureList->getCurrent();



					if ((tempStructure->getItemID() == Structure_Refinery) && (tempStructure->getOwner() == owner))

					{

						tempRefinery = static_cast<RefineryClass*>(tempStructure);

						tempDistance = distance_from(&location, &tempRefinery->getClosestPoint(&location));

						tempNumBookings = tempRefinery->getNumBookings();

						if (tempNumBookings < leastNumBookings)

						{

							leastNumBookings = tempNumBookings;

							closestLeastBookedRefineryDistance = tempDistance;

							bestRefinery = tempRefinery;

						}

						else if (tempNumBookings == leastNumBookings)

						{

							if (tempDistance < closestLeastBookedRefineryDistance)

							{

								closestLeastBookedRefineryDistance = tempDistance;

								bestRefinery = tempRefinery;

							}

						}

					}

					structureList->nextLink();

				}



				if (bestRefinery)

				{

					if (settings.playerType != CLIENT)

						netDoCommand(bestRefinery, &bestRefinery->getClosestPoint(&location), true);

					bestRefinery->startAnimate();

				}

			}

		}

		else if (harvesting && !hasBookedCarrier() && (blockDistance(&location, &destination) > 10.0))

			requestCarryall();

	}

}



void HarvesterClass::deploy(COORDTYPE* newLocation)

{

	if (map->cellExists(newLocation))

	{

		UnitClass::deploy(newLocation);

		if (spice == 0)

		{

			if (map->findSpice(&destination, &guardPoint))

			{

				harvesting = true;

				guardPoint = destination;

			}

			else

				harvesting = false;

		}

	}

}



void HarvesterClass::destroy()

{

	setTarget(NULL);

	UnitClass::destroy();

}



void HarvesterClass::drawSelectionBox()

{

	SDL_Rect	dest;

	SDL_Surface* selectionBox = (SDL_Surface*)(dataFile[UI_SelectionBox].dat);

	dest.x = getDrawnX() + imageW/2 - selectionBox->w/2;

	dest.y = getDrawnY() + imageH/2 - selectionBox->h/2;

	dest.w = selectionBox->w;

	dest.h = selectionBox->h;

	SDL_BlitSurface(selectionBox, NULL, screen, &dest);



	dest.x = getDrawnX() + imageW/2 - selectionBox->w/2;

	dest.y = getDrawnY() + imageH/2 - selectionBox->h/2;

	drawhline(screen, dest.x+1, dest.y-1, dest.x+1 + ((int)(((double)health/(double)maxHealth)*(selectionBox->w-3))), getHealthColour());



	if ((getOwner() == thisPlayer) && (spice > 0.0))

	{

		dest.x = getDrawnX() + imageW/2 - selectionBox->w/2;

		dest.y = getDrawnY() + imageH/2 - selectionBox->h/2;

		drawhline(screen, dest.x+1, dest.y-2, dest.x+1 + ((int)(((double)spice/HARVESTERMAXSPICE)*(selectionBox->w-3))), COLOUR_ORANGE);

							//want it to start in one from edges		finish one from right edge

	}

}



void HarvesterClass::handleDamage(int damage, ObjectClass* damager)

{

	if (!wasDestroyed())

	{

		ObjectClass::handleDamage(damage, damager);

	    

		if (!target && !forced && damager && canAttack(damager)

			&& (attackMode != SCOUT))

			setTarget(damager);

	}

}



void HarvesterClass::harvesterReturn()

{

	returningToRefinery = true;

	harvesting = false;

}



void HarvesterClass::setAmountOfSpice(double newSpice)

{

	if ((newSpice >=0) && (newSpice <= HARVESTERMAXSPICE))

		spice = newSpice;

}



void HarvesterClass::setDestination(int newX, int newY)

{

	ObjectClass::setDestination(newX, newY);



	harvesting = (map->cellExists(newX, newY) && map->cell[newX][newY].hasSpice());

}



void HarvesterClass::setTarget(ObjectClass* newTarget)

{

	if (returningToRefinery && target && (target->getItemID() == Structure_Refinery))

	{

		((RefineryClass*)target)->unBook();

		returningToRefinery = false;

	}



	UnitClass::setTarget(newTarget);



	if (target && (target->getOwner() == getOwner()) && (target->getItemID() == Structure_Refinery))

	{

		((RefineryClass*)target)->book();

		returningToRefinery = true;

	}

}



void HarvesterClass::setReturned()

{

	if (selected)

		removeFromSelectionLists();

	map->removeObjectFromMap(this);



	((RefineryClass*)target)->assignHarvester(this);

	returningToRefinery = false;

	moving = false;

	respondable = false;

	setActive(false);

	setLocation(NONE, NONE);

	setVisible(VIS_ALL, false);

}



void HarvesterClass::move()

{

	UnitClass::move();

	if (active && !moving)

	{

		if (harvesting)

		{

			//every now and then move around whilst harvesting

			if ((settings.playerType != CLIENT) && !forced && map->cell[location.x][location.y].hasSpice() && (getRandomInt(0, RANDOMHARVESTMOVE) == 0))

			{

				map->findSpice(&destination, &location);

				netDoCommand(NULL, &destination, false);

			}



			if (location.x == destination.x && location.y == destination.y)

			{

				if (spice < HARVESTERMAXSPICE)

				{

					if (map->cell[location.x][location.y].hasSpice())

					{

						spice += map->cell[location.x][location.y].harvestSpice();



						if (!map->cell[location.x][location.y].hasSpice())

							map->removeSpice(location.x, location.y);

					}

					else if (!map->findSpice(&destination, &location))

						harvesterReturn();

					else if (settings.playerType != CLIENT)

					{

						setGuardPoint(&destination);

						setDestination(guardPoint.x, guardPoint.y);

						netDoCommand(NULL, &destination, false);

					}

				}

				else

					harvesterReturn();

			}

		}

	}

}



bool HarvesterClass::canAttack(ObjectClass* object)

{

	return (!object->wasDestroyed() && object->isInfantry() && (object->getOwner()->getTeam() != owner->getTeam()) && object->isVisible(getOwner()->getTeam()));

}



double HarvesterClass::extractSpice()

{

	double oldSpice = spice;

	

	if ((spice - HARVESTEREXTRACTSPEED) >= 0)

		spice -= HARVESTEREXTRACTSPEED;

	else

		spice = 0;

	

	return (oldSpice - spice);

}

