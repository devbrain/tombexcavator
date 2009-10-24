#include "../sand.h"
#include "../Items.h"

Carryall::Carryall(PlayerClass* newOwner) : AirUnit(newOwner)
{
	baseID = itemID = Unit_Carryall;

	armour = 2;
	speed = 1.8;
	maxHealth = 100;
	health = maxHealth;
	radius = 8;
	viewRange = 8;
	weaponRange = 0;

	nextSpotAngle = drawnAngle;
	turnSpeed = 0.125;
	weaponRange = 0;
	viewRange = DEFAULT_UNITVIEWRANGE;

	attackMode = DEFENSIVE;
	canAttackStuff = false;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h/2;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	aDropOfferer = false;
	booked = false;
	droppedOffCargo = false;
	numCargo = 0;

	curFlyPoint = 0;

	//setDestination(&flyPoints[0]);
	//setDestination(&guardPoint);

	idle = true;
	firstRun = true;
	owned = true;

}
Carryall::~Carryall()
{
}

void Carryall::checkPos()
{
	AirUnit::checkPos();
	if (active)
	{
		if (aDropOfferer)
		{
			if (droppedOffCargo && ((location.x == 0) || (location.x == map->sizeX-1) || (location.y == 0) || (location.y == map->sizeY-1)))
			{
				setVisible(VIS_ALL, false);
				netDestroy();
			}
		}

		if (hasCargo() && !hasATarget() && (location.x == destination.x) && (location.y == destination.y))
		{
			pickedUpUnits.reset();
			while (pickedUpUnits.currentNotNull())
			{
				deployUnit((UnitClass*)pickedUpUnits.getCurrent());
				pickedUpUnits.reset();
			}

			setTarget(NULL);
			setDestination(&guardPoint);
			//setDestination(&flyPoints[0]);
			//showMessage("should have dropped stuff!- checkPos()");
			idle = true;
		}
		else if(idle && !firstRun)
		{
			//fly around const yard
			COORDTYPE point;
			int looped = 0;
			point = this->getClosestPoint(&location);

			//showMessage("finding new dest");
			if(point.x == guardPoint.x && point.y == guardPoint.y)
			{
				//arrived at point, move to next
				curFlyPoint++;

				if(curFlyPoint >= 8)
				{
					curFlyPoint = 0;
				}

				while(!(map->cellExists(flyPoints[curFlyPoint].x, flyPoints[curFlyPoint].y)) && looped <= 2)
				{
					curFlyPoint++;

					if(curFlyPoint >= 8)
					{
						curFlyPoint = 0;
						looped++;
					}
				}

				

				setGuardPoint(&flyPoints[curFlyPoint]);
				setDestination(&guardPoint);
			}
		}
		else if(firstRun && owned)
		{
			findConstYard();
			setGuardPoint(&constYardPoint);
			setDestination(&guardPoint);
			firstRun = false;
		}
	}
	

}

void Carryall::deployUnit(UnitClass* unit)
{
	bool found = false;
	pickedUpUnits.reset();
	while (pickedUpUnits.currentNotNull())
	{
		if (pickedUpUnits.getCurrent() == unit)
		{
			pickedUpUnits.removeCurrent();
			found = true;
		}
		else
			pickedUpUnits.nextLink();
	}

	if (found)
	{
		if (map->getCell(&location)->hasANonInfantryGroundObject())
		{
			ObjectClass* object = map->getCell(&location)->getNonInfantryGroundObject();
			if (object->getOwner() == getOwner())
			{
				if (object->getItemID() == Structure_RepairYard)
				{
					//showMessage("arrived at repair yard");
					if (((RepairYardClass*)object)->isFree())
					{
						unit->setTarget(object);
						unit->setGettingRepaired();
						unit = NULL;
					}
					else
						((RepairYardClass*)object)->unBook();
				}
				else if ((object->getItemID() == Structure_Refinery) && (unit->getItemID() == Unit_Harvester))
				{
					if (((RefineryClass*)object)->isFree())
					{
						((HarvesterClass*)unit)->setTarget(object);
						((HarvesterClass*)unit)->setReturned();
						unit = NULL;
						goingToRepairYard = false;
					}
				}
			}
		}
		
		if (unit)
		{
			unit->setAngle(drawnAngle);
			unit->deploy(&map->findDeploySpot(&location, NULL, NULL, unit));
		}

		if (--numCargo == 0)
		{
			if (!aDropOfferer)
				booked = false;
			droppedOffCargo = true;
			drawnFrame = 0;
			checkTimer = 0;
		}

	}
}

void Carryall::destroy()
{
	UnitClass::destroy();

	pickedUpUnits.reset();
	while (pickedUpUnits.currentNotNull())
		((UnitClass*)pickedUpUnits.removeCurrent())->netDestroy();
}

void Carryall::engageTarget()
{

	if (target->wasDestroyed()
		|| (!target->isVisible(getOwner()->getTeam()))
		|| (target->isAGroundUnit()
			&& !((GroundUnit*)target)->isAwaitingPickup())
		|| (target->getOwner()->getTeam() != owner->getTeam()))
	{
		setTarget(NULL);
		if (!hasCargo())
		{
			booked = false;
			setDestination(&guardPoint);
			nextSpotFound = false;
			//showMessage("going to guard point-engage1");
		}
	}
	else
	{
		COORDTYPE targetLocation = target->getClosestPoint(&location);
		COORDTYPE realLocation, realDestination;
		realLocation.x = round(realX);
		realLocation.y = round(realY);
		realDestination.x = targetLocation.x * BLOCKSIZE + BLOCKSIZE/2;
		realDestination.y = targetLocation.y * BLOCKSIZE + BLOCKSIZE/2;
		targetAngle = round((double)NUM_ANGLES*dest_angle(&realLocation, &realDestination)/256.0);
		if (targetAngle == 8)
			targetAngle = 0;

		targetDistance = blockDistance(&location, &targetLocation);

		if (targetDistance <= weaponRange)
		{
			if (target->isAUnit())
				targetAngle = ((GroundUnit*)target)->getAngle();

			if (drawnAngle == targetAngle)
			{
				if (target->isAGroundUnit())
					pickupTarget();
				else if (hasCargo() && target->isAStructure())
				{
					pickedUpUnits.reset();
					while (pickedUpUnits.currentNotNull())
					{
						deployUnit((UnitClass*)pickedUpUnits.getCurrent());
						pickedUpUnits.reset();
					}

					setTarget(NULL);
					setDestination(&guardPoint);
					//showMessage("retuning to guard point, should have dropped stuff-engage");
				}
			}
		}
		else
			setDestination(&targetLocation);
	}
}

void Carryall::giveCargo(UnitClass* newObject)
{
	booked = true;
	numCargo++;

	//if (numCargo <= maxCargo)
	{
		pickedUpUnits.insertLast(newObject);
		((UnitClass*)newObject)->setPickedUp(this);
		if (getItemID() != Unit_Frigate)
			drawnFrame = 1;

		droppedOffCargo = false;
	}
}

void Carryall::handleActionCommand(int xPos, int yPos)
{
	if (respondable && !aDropOfferer && !booked)
	{
		COORDTYPE		dest;
		ObjectClass		*tempTarget = NULL;

		dest.x = xPos;
		dest.y = yPos;

		if (target && target->isAStructure())
		{
			if (goingToRepairYard)
			{
				((RepairYardClass*)target)->unBook();
				goingToRepairYard = false;
				target = NULL;
			}
			else if (target->getItemID() == Structure_Refinery)
			{
				((RefineryClass*)target)->unBook();
				target = NULL;
			}
		}	
		
		if (map->cell[xPos][yPos].hasAnObject())
		{
			tempTarget = map->cell[xPos][yPos].getObject();

			if (tempTarget == this)
			{
				pickedUpUnits.reset();
				while (pickedUpUnits.currentNotNull())
				{
					deployUnit((UnitClass*)pickedUpUnits.getCurrent());
					pickedUpUnits.reset();
				}
			}
			else
			{
				if (tempTarget->isAStructure())
				{
					int targetID = tempTarget->getItemID();
					if (targetID == Structure_RepairYard)
					{
						((RepairYardClass*)tempTarget)->book();
						goingToRepairYard = true;
					}
					else if (targetID == Structure_Refinery)
						((RefineryClass*)tempTarget)->book();
				}
			}
		}

		netDoCommand(tempTarget, &dest, true);
		pathList.clearList();
	}
}

void Carryall::pickupTarget()
{
	if (target->isAGroundUnit())
	{
		if (target->hasATarget() || ((((GroundUnit*)target)->getGuardPoint()->x != target->getLocation()->x) || (((GroundUnit*)target)->getGuardPoint()->y != target->getLocation()->y)) || ((GroundUnit*)target)->isBadlyDamaged())
		{
			bool gotHim = false;

			if(((GroundUnit*)target)->isBadlyDamaged())
			{
				((GroundUnit*)target)->repair();
			}

			ObjectClass* newTarget = target->hasATarget() ? target->getTarget() : NULL;
			
			if (!((GroundUnit*)target)->isMoving())
			{
				pickedUpUnits.insertLast(target);
				((GroundUnit*)target)->setPickedUp(this);
				drawnFrame = 1;
				numCargo++;
				booked = true;
				gotHim = true;
			}

			if (gotHim)
			{
				if ((target->getItemID() == Unit_Harvester) && newTarget && ((newTarget->getItemID() == Structure_Refinery) || (newTarget->getItemID() == Structure_RepairYard)))
				{
					//showMessage("Harvester pickup");
					setTarget(newTarget);
					setDestination(&target->getClosestPoint(&location));					
				}
				else if (((GroundUnit*)target)->getDestination()->x != NONE)
				{
					//showMessage("Lazy Pickup");
					setDestination(((GroundUnit*)target)->getDestination());
				}

				clearPath();
				checkTimer = 0;
			}
		}
		else
		{
			((GroundUnit*)target)->setAwaitingPickup(false);
			setTarget(NULL);
			//showMessage("Not suitable for pickup");
		}
	}
}

void Carryall::loadCargo(FILE* fp)
{
	int				i, num, objectID;
	ObjectClass*	object;
	
	fread(&num, sizeof(int), 1, fp);

	for (i = 0; i < num; i++)
	{
		fread(&objectID, sizeof(int), 1, fp);
		object = map->findObjectWidthID(objectID, NONE, NONE);
		if (object && object->isAUnit())
			giveCargo((UnitClass*)object);		
	}
}

void Carryall::saveCargo(FILE* fp)
{
	int	num;
	
	num = pickedUpUnits.getNumElements();
	fwrite(&num, sizeof(int), 1, fp);

	pickedUpUnits.reset();
	while (pickedUpUnits.currentNotNull())
	{
		num = ((UnitClass*)pickedUpUnits.getCurrent())->getObjectID();
		fwrite(&num, sizeof(int), 1, fp);
		pickedUpUnits.nextLink();
	}
}

void Carryall::setTarget(ObjectClass* newTarget)
{
	if (target && targetFriendly && target->isAGroundUnit() && (((GroundUnit*)target)->getCarrier() == this))
		((GroundUnit*)target)->bookCarrier(NULL);
	UnitClass::setTarget(newTarget);

	if (target && targetFriendly && target->isAGroundUnit())
		((GroundUnit*)target)->setAwaitingPickup(true);

	booked = (target != NULL);
}

void Carryall::targeting()
{
	if (target)
		engageTarget();
}

void Carryall::turn()
{
	if (!moving)
	{
		int wantedAngle;
		if (target && (targetDistance <= weaponRange))
			wantedAngle = targetAngle;
		else
			wantedAngle = nextSpotAngle;

		if (wantedAngle != NONE)
		{
			if (justStoppedMoving)
			{
				angle = wantedAngle;
				drawnAngle = round(angle);
			}
			else
			{
				double	angleLeft = 0,
						angleRight = 0;

				if (angle > wantedAngle)
				{
					angleRight = angle - wantedAngle;
					angleLeft = fabs(8-angle)+wantedAngle;
				}
				else if (angle < wantedAngle)
				{
					angleRight = abs(8-wantedAngle) + angle;
					angleLeft = wantedAngle - angle;
				}

				if (angleLeft <= angleRight)
					turnLeft();
				else
					turnRight();
			}
		}
	}
}

bool Carryall::hasCargo()
{
	return (numCargo > 0);
}

void Carryall::findConstYard()
{
	if (!structureList->isEmpty())
	{
//		showMessage("finding const yard");
			double	closestYardDistance = 1000000,//same deal
					tempDistance = 0;

			ConstructionYardClass	*bestYard = NULL,
									*tempYard;
			StructureClass	*tempStructure;

			structureList->reset();
			while(structureList->currentNotNull())
			{
				tempStructure = (StructureClass*)structureList->getCurrent();

				if ((tempStructure->getItemID() == Structure_ConstructionYard) && (tempStructure->getOwner() == owner))
				{
					tempYard = ((ConstructionYardClass*)tempStructure);
					tempDistance = distance_from(&location, &tempYard->getClosestPoint(&location));
						
					if (tempDistance < closestYardDistance)
					{
						closestYardDistance = tempDistance;
						bestYard = tempYard;
					}
					
				}
				structureList->nextLink();
			}

			if (bestYard)
			{
				constYardPoint = bestYard->getClosestPoint(&location);
//				setGuardPoint(&constYardPoint);
//				showMessage("found a close const yard");

			}
			else
			{
				constYardPoint = guardPoint;
			}

			flyPoints[0].x = constYardPoint.x + 8;
			flyPoints[0].y = constYardPoint.y;

			flyPoints[1].x = constYardPoint.x + 4;
			flyPoints[1].y = constYardPoint.y + 4;

			flyPoints[2].x = constYardPoint.x;
			flyPoints[2].y = constYardPoint.y + 8;

			flyPoints[3].x = constYardPoint.x - 4;
			flyPoints[3].y = constYardPoint.y + 4;

			flyPoints[4].x = constYardPoint.x - 8;
			flyPoints[4].y = constYardPoint.y;

			flyPoints[5].x = constYardPoint.x - 4;
			flyPoints[5].y = constYardPoint.y - 4;

			flyPoints[6].x = constYardPoint.x;
			flyPoints[6].y = constYardPoint.y - 8;

			flyPoints[7].x = constYardPoint.x + 4;
			flyPoints[7].y = constYardPoint.y - 4;
		
	}
}

