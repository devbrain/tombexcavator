#include "../sand.h"
#include "../Items.h"

UnitClass::UnitClass(PlayerClass* newOwner) : ObjectClass(newOwner)
{

	aUnit = true;



	interfaceContainer = &unitInterface;



	realOwner = newOwner;

	deathFrame = Object_SmallUnitExplosion;

	numDeathFrames = 2;



	destination.x = destination.y = NONE;

	guardPoint.x = guardPoint.y = NONE;

	nextSpot.x = nextSpot.y = NONE;


	xSpeed = 0; ySpeed = 0;
	angle = drawnAngle;
	destAngle = drawnAngle;
	deviationTimer = 0;
	frameTime = 25;
	nextSpotAngle = drawnAngle;
	turnSpeed = 0.0625;
	primaryWeaponTimer = 0;
	secondaryWeaponTimer = 0;
	targetAngle = NONE;
	targetDistance = 0;
	weaponRange = 0;
	viewRange = DEFAULT_UNITVIEWRANGE;

	numWeapons = 0;

	findTargetTimer = 0;

	primaryWeaponTimer = 0;

	secondaryWeaponTimer = -1;


	attacking = false;
	canAttackStuff = true;
	moving = false;

	pickedUp = false;

	justStoppedMoving = false;
	turning = false;
	tracked = false;
	turreted = false;
	goingToRepairYard = false;
	nextSpotFound = false;
	respondable = true;

	attackMode = DEFENSIVE;



	speed = 0.0;

	speedCap = NONE;



	numConfirmSounds = 2;

	numDestroySounds = 1;

	numSelectSounds = 1;

	confirmSound[0] = Sound_acknowledged;

	confirmSound[1] = Sound_affirmative;

	destroySound[0] = Sound_smallExplosion;

	selectSound[0] = Sound_reporting;

/*	if (getOwner() == thisPlayer)

	{

		unitsCreated++;

		char temp123[50];

		sprintf(temp123, "units:%d, structures:%d", unitsCreated, structuresCreated);

		showMessage(temp123);

	}*/

}
UnitClass::~UnitClass()
{

	pathList.clearList();

	//printf("deleting type %d.\n", itemID);fflush(stdout);

	removeFromSelectionLists();



	if (map->cellExists(&location))

		map->getCell(&location)->unassignDeadObject(this);



	realOwner->decrementUnits(itemID);



	/*if (getOwner() == thisPlayer)

	{

		unitsCreated++;

		char temp123[50];

		sprintf(temp123, "units:%d, structures:%d", unitsCreated, structuresCreated);

		showMessage(temp123);

	}*/

}


void UnitClass::attack()

{

	if (numWeapons)

	{

		COORDTYPE centrePoint, targetCentrePoint;

		getCentrePoint(&centrePoint);

		target->getClosestCentrePoint(&targetCentrePoint, &location);

		if (primaryWeaponTimer == 0)

		{

			bulletList->insertLast(new BulletClass(this, &centrePoint, &targetCentrePoint, bulletType[0], target->isAFlyingUnit()));

			soundPlayer->playSoundAt(attackSound[0], &location);



			primaryWeaponTimer = primaryWeaponReloadTime;

			secondaryWeaponTimer = 15;

		}



		if ((numWeapons == 2) && (secondaryWeaponTimer == 0) && !badlyDamaged)

		{

			bulletList->insertLast(new BulletClass(this, &centrePoint, &targetCentrePoint, bulletType[0], target->isAFlyingUnit()));

			soundPlayer->playSoundAt(attackSound[0], &location);

			secondaryWeaponTimer = -1;

		}

	}

}



void UnitClass::blitToScreen()
{

	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	
	if (getItemID() == Unit_Frigate)
		itemID = Unit_Frigate;
	if (!destroyed)
	{

		source.x = drawnAngle*imageW;

		source.y = drawnFrame*imageH;


		SDL_BlitSurface(graphic, &source, screen, &dest);
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



void UnitClass::deploy(COORDTYPE* newLocation)
{
	if (map->cellExists(newLocation))
	{
		setLocation(newLocation);

		if ((guardPoint.x == NONE) || (guardPoint.y == NONE))

			guardPoint = location;

		setDestination(&guardPoint);

		pickedUp = false;

		setRespondable(true);

		setActive(true);

		setVisible(VIS_ALL, true);

	}

}



void UnitClass::destroy()

{

	if (!destroyed)

	{

		setTarget(NULL);

		map->removeObjectFromMap(this);	//no map point will reference now



		SDL_FreeSurface(graphic);

		destroyed = true;

		respondable = false;

		graphic = (SDL_Surface*)(dataFile[deathFrame].dat);

		imageW = graphic->w/numDeathFrames;

		imageH = graphic->h;

		xOffset = (imageW - BLOCKSIZE)/2;		    //this is where it actually draws the graphic

		yOffset = (imageH - BLOCKSIZE)/2;		    //cause it draws at top left, not middle

		frameTimer = frameTime;



		deathFrame = 0;



		if (isVisible(getOwner()->getTeam()) && numDestroySounds)

			soundPlayer->playSoundAt(destroySound[getRandomInt(0, numDestroySounds-1)], &location);



		if (map->cellExists(&location))

			map->getCell(&location)->assignDeadObject(this);

	}

}



void UnitClass::deviate(PlayerClass* newOwner)

{

	removeFromSelectionLists();

	setTarget(NULL);

	setGuardPoint(&location);

	setDestination(&location);

	clearPath();

	nextSpotFound = false;

	setAttackMode(AGGRESSIVE);



	owner = newOwner;

	SDL_FreeSurface(graphic);
	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());



	deviationTimer = DEVIATIONTIME;

}



void UnitClass::drawSelectionBox()

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

}	//want it to start in one from edges		finish one from right edge



void UnitClass::engageTarget()

{

	if (target->wasDestroyed()

		|| ((!canAttack(target)

			|| (!forced && !isInAttackModeRange(target)))

		&& !targetFriendly))

	{

		findTargetTimer = 0;

		forced = false;

		setTarget(NULL);



		//if (attackMode != DEFENSIVE)

		//	setGuardPoint(&location);

		setDestination(&guardPoint);

		nextSpotFound = false;

		pathList.clearList();

	}

	else

	{

		COORDTYPE targetLocation = target->getClosestPoint(&location);

		targetDistance = blockDistance(&location, &targetLocation);

		

		if ((destination.x != targetLocation.x)	//targets location has moved, recalculate path

			&& (destination.y != targetLocation.y))

		{

			pathList.clearList();

			nextSpotFound = false;

		}



		if (forced)

			setDestination(&targetLocation);



		targetAngle = round(8.0/256.0*dest_angle(&location, &targetLocation));

		if (targetAngle == 8)

			targetAngle = 0;



		if (targetDistance <= weaponRange)

		{

			if (!targetFriendly)

			{

				if (isTracked() && target->isInfantry() && (forced || (attackMode == AGGRESSIVE) || (attackMode == DEFENSIVE)))

					setDestination(&targetLocation);

				else

				{

					setDestination(&location);

					pathList.clearList();

					nextSpotFound = false;

				}

				if (drawnAngle == targetAngle)

					attack();

			}

		}

		else

			setDestination(&targetLocation);

	}

}

void UnitClass::move()
{
	if(!moving && getRandomInt(0,40) == 0)
		map->viewMap(owner->getTeam(), &location, getViewRange() ); 

	if (moving)

	{
		COORDTYPE oldLocation = location;

		if(!badlyDamaged)
		{
			realX += xSpeed;
			realY += ySpeed;
		}
		else
		{
			realX += xSpeed/2;
			realY += ySpeed/2;
		}
		// if vehicle is half way out of old cell
		if ((abs(location.x*BLOCKSIZE - (int)realX + BLOCKSIZE/2) > BLOCKSIZE/2)
			|| (abs(location.y*BLOCKSIZE - (int)realY + BLOCKSIZE/2) > BLOCKSIZE/2))
		{

			unassignFromMap(&oldLocation);	//let something else go in



			// if vehicle is out of old cell

			if ((abs(location.x*BLOCKSIZE - (int)realX + BLOCKSIZE/2) > BLOCKSIZE)
			|| (abs(location.y*BLOCKSIZE - (int)realY + BLOCKSIZE/2) > BLOCKSIZE))
			{

				location.x = nextSpot.x;
				location.y = nextSpot.y;

				if (location.x == destination.x && location.y == destination.y)

					setForced(false);


				moving = false;

				justStoppedMoving = true;

				map->viewMap(owner->getTeam(), &location, getViewRange() );
			}
		}

	}

	else

		justStoppedMoving = false;



	checkPos();
}



void UnitClass::navigate()
{

	if (!moving)

	{

		if ((location.x != destination.x) || (location.y != destination.y))

		{

			if (!nextSpotFound)
			{

				if (nextSpotAngle == drawnAngle)

				{
					if (pathList.isEmpty() && (checkTimer == 0))
					{
						checkTimer = 100;

						if (!AStarSearch() && (++noCloserPointCount >= 3)
							&& ((location.x != oldLocation.x) || (location.y != oldLocation.y)))
						{	//try searching for a path a number of times then give up
							if (target && targetFriendly
								&& (target->getItemID() != Structure_RepairYard)
								&& ((target->getItemID() != Structure_Refinery)
									|| (getItemID() != Unit_Harvester)))
								setTarget(NULL);
							setDestination(&location);	//can't get any closer, give up
							forced = false;
							speedCap = NONE;
						}
					}

					if (!pathList.isEmpty())
					{
						nextSpot = *(COORDTYPE*)pathList.removeFirst();
						nextSpotFound = true;
						checkTimer = 0;
						noCloserPointCount = 0;
					}
				}

			}

			else
			{
				int tempAngle;
				if ((tempAngle = map->getPosAngle(&location, &nextSpot)) != NONE)
					nextSpotAngle = tempAngle;
				if (!canPass(nextSpot.x, nextSpot.y))

				{

					nextSpotFound = false;

					pathList.clearList();

				}

				else if (drawnAngle == nextSpotAngle)
				{
					moving = true;
					nextSpotFound = false;



					assignToMap(&nextSpot);

					angle = drawnAngle;

					setSpeeds();
				}
			}

		}

		else if (!target)	//not moving and not wanting to go anywhere, do some random turning

		{

			//if (getRandomInt(0, RANDOMTURNTIMER) == 0)

			//	nextSpotAngle = getRandomInt(0, 7);	//choose a random one of the eight possible angles

		}

	}
}



void UnitClass::frameChange()

{

	deathFrame++;

	if (deathFrame < numDeathFrames)

		frameTimer = frameTime;

}



void UnitClass::handleActionCommand(int xPos, int yPos)
{
	if (respondable)
	{
		COORDTYPE		dest;
		ObjectClass*	tempTarget = NULL;

		if (map->cellExists(xPos, yPos))
		{
			dest.x = xPos;
			dest.y = yPos;

			if (map->cell[xPos][yPos].hasAnObject())
				tempTarget = map->cell[xPos][yPos].getObject();

			netDoCommand(tempTarget, &dest, true);
		}
	}

}



bool UnitClass::isInAttackModeRange(ObjectClass* object)	//optimised

{

	//COORDTYPE objectLocation = object->getClosestPoint(&location);

	//double objectDistance = blockDistance(&location, &objectLocation);



	switch (attackMode)

	{

		case (STANDGROUND): //if the distance to this object is in weapon range, its attackable

		{

			return (/*objectDistance*/blockDistance(&location, &object->getClosestPoint(&location)) <= weaponRange);

			break;

		}

		case (DEFENSIVE): //if the object is not further then twice the guard range plus weapon range, and this unit is not further then twice the guardrange away, its attackable

		{

			COORDTYPE objectLocation = object->getClosestPoint(&location);

			return ((blockDistance(&guardPoint, &objectLocation) + /*objectDistance*/blockDistance(&location, &objectLocation)) <= (2*guardRange + weaponRange));

			break;

		}

		case (AGGRESSIVE):	//don't care go, its attackable

			return true;

			break;

		default:

			return false;

			break;

	}



	return false;

}



void UnitClass::netDeviate(PlayerClass* newOwner)

{

	if (settings.playerType != CLIENT)

	{

		deviate(newOwner);

		if (settings.playerType == SERVER)

			netServerDeviate(getObjectID(), getX(), getY(), newOwner->getPlayerNumber());

	}

}



void UnitClass::netDoCommand(ObjectClass* newTarget, COORDTYPE* dest, bool forced)
{
	if (settings.playerType != CLIENT)
	{
		if ((newTarget == NULL) || (newTarget->getOwner() != getOwner())
			|| ((newTarget->getItemID() != Structure_RepairYard)
				&& ((newTarget->getItemID() != Structure_Refinery)
					|| (getItemID() != Unit_Harvester))))
			setGuardPoint(dest);

		if ((dest->x != destination.x) || (dest->y != destination.y))
		{
			clearPath();
			checkTimer = 0;
			findTargetTimer = 0;
			nextSpotFound = false;
			resetStartPoint();
		}
	}

	ObjectClass::netDoCommand(newTarget, dest, forced);
}



void UnitClass::netDoConsistancy()
{
	if (settings.playerType == SERVER)
		netServerConsistancyMessage(objectID, &location, drawnAngle, NONE);
}



void UnitClass::netSetAttackMode(ATTACKTYPE newAttackMode)

{

	if (settings.playerType != CLIENT)

	{

		setAttackMode(newAttackMode);

		if (settings.playerType == SERVER)

			netServerSetAttackMode(getObjectID(), getX(), getY(), newAttackMode);

	}

	else

		netClientSetAttackMode(getObjectID(), getX(), getY(), newAttackMode);

}



void UnitClass::repair()

{

	if (health < maxHealth)

	{

		//find a repair yard to return to

		if (!structureList->isEmpty())

		{

			int	leastNumBookings = 1000000,//huge amount so repairyard couldn't be possible

				tempNumBookings = 0;



			double	closestLeastBookedRepairYardDistance = 1000000,//same deal

					tempDistance = 0;



			RepairYardClass	*bestRepairYard = NULL,

							*tempRepairYard;

			StructureClass	*tempStructure;



			structureList->reset();

			while(structureList->currentNotNull())

			{

				tempStructure = (StructureClass*)structureList->getCurrent();



				if ((tempStructure->getItemID() == Structure_RepairYard) && (tempStructure->getOwner() == owner))

				{

					tempRepairYard = ((RepairYardClass*)tempStructure);

					tempDistance = distance_from(&location, &tempRepairYard->getClosestPoint(&location));

					tempNumBookings = tempRepairYard->getNumBookings();

					if (tempNumBookings < leastNumBookings)

					{

						leastNumBookings = tempNumBookings;

						bestRepairYard = tempRepairYard;

						closestLeastBookedRepairYardDistance = tempDistance;

					}

					else if (tempNumBookings == leastNumBookings)

					{

						if (tempDistance < closestLeastBookedRepairYardDistance)

						{

							closestLeastBookedRepairYardDistance = tempDistance;

							bestRepairYard = tempRepairYard;

						}

					}

				}

				structureList->nextLink();

			}



			if (bestRepairYard)

			{

				if (settings.playerType != CLIENT)

					netDoCommand(bestRepairYard, &bestRepairYard->getClosestPoint(&location), true);

			}

		}

	}

}



void UnitClass::resetStartPoint()

{

	oldLocation = location;

}



void UnitClass::setAngle(int newAngle)

{

	if (!moving && (newAngle >= 0) && (newAngle < NUM_ANGLES))

	{

		angle = drawnAngle = newAngle;

		nextSpotAngle = drawnAngle; 

		nextSpotFound = false;

	}

}



void UnitClass::setAttackMode(ATTACKTYPE newAttackMode)

{

	if ((newAttackMode >= SCOUT) && (newAttackMode <= AGGRESSIVE))

		attackMode = newAttackMode;

}



void UnitClass::setDeviationTime(int newTime)

{

	if ((newTime > 1) && (newTime <= DEVIATIONTIME))

		deviationTimer = newTime;

}



void UnitClass::setGettingRepaired()

{

	if (target && (target->getItemID() == Structure_RepairYard))

	{

		if (selected)

			removeFromSelectionLists();

		map->removeObjectFromMap(this);



		((RepairYardClass*)target)->assignUnit(this);

		respondable = false;

		setActive(false);

		setVisible(VIS_ALL, false);

		goingToRepairYard = false;

		badlyDamaged = false;

		smokeFrame = NONE;

		setTarget(NULL);

		//setLocation(NONE, NONE);

		setDestination(&location);

		nextSpotAngle = DOWN;

	}

}



void UnitClass::setGuardPoint(COORDTYPE* newGuardPoint)

{

	setGuardPoint(newGuardPoint->x, newGuardPoint->y);

}



void UnitClass::setGuardPoint(int newX, int newY)

{

	if (map->cellExists(newX, newY) || ((newX == NONE) && (newY == NONE)))

	{

		guardPoint.x = newX;

		guardPoint.y = newY;

	}

}

void UnitClass::setLocation(int xPos, int yPos)

{

	if (map->cellExists(xPos, yPos) || ((xPos == NONE) && (yPos == NONE)))

	{

		ObjectClass::setLocation(xPos, yPos);

		realX += BLOCKSIZE/2;

		realY += BLOCKSIZE/2;

		moving = false;

		nextSpotFound = false;

		nextSpotAngle = drawnAngle;

		pickedUp = false;

		setTarget(NULL);

		clearPath();

		noCloserPointCount = 0;

	}

}



void UnitClass::setPickedUp(UnitClass* newCarrier)

{

	if (selected)

		removeFromSelectionLists();

	map->removeObjectFromMap(this);



	if (goingToRepairYard)

		((RepairYardClass*)target)->unBook();

	target = newCarrier;

	goingToRepairYard = false;

	moving = false;

	nextSpotFound = false;

	pickedUp = true;

	respondable = false;

	setActive(false);

	setVisible(VIS_ALL, false);

}



void UnitClass::setSpeeds()

{

	double maxSpeed = speed;



	if (!isAFlyingUnit())

		speed += speed*(1.0 - map->getCell(&location)->getDifficulty());



	if (badlyDamaged)

		speed *= HEAVILYDAMAGEDSPEEDMULTIPLIER;



	if ((speedCap > 0) && (speedCap < speed))

		speed = speedCap;



	switch (drawnAngle)

	{

	case (LEFT): xSpeed = -speed; ySpeed = 0; break;

	case (LEFTUP): xSpeed = -speed*DIAGONALSPEEDCONST; ySpeed = xSpeed; break;

	case (UP): xSpeed = 0; ySpeed = -speed; break;

	case (RIGHTUP): xSpeed = speed*DIAGONALSPEEDCONST; ySpeed = -xSpeed; break;

	case (RIGHT): xSpeed = speed; ySpeed = 0; break;

	case (RIGHTDOWN): xSpeed = speed*DIAGONALSPEEDCONST; ySpeed = xSpeed; break;

	case (DOWN): xSpeed = 0; ySpeed = speed; break;

	case (LEFTDOWN): xSpeed = -speed*DIAGONALSPEEDCONST; ySpeed = -xSpeed;

	}



	speed = maxSpeed;

}



void UnitClass::setTarget(ObjectClass* newTarget)
{
	if (goingToRepairYard && target && (target->getItemID() == Structure_RepairYard))
	{
		((RepairYardClass*)target)->unBook();
		goingToRepairYard = false;
	}

	ObjectClass::setTarget(newTarget);

	if (target && (target->getOwner() == getOwner()) && (target->getItemID() == Structure_RepairYard))
	{
		((RepairYardClass*)target)->book();
		goingToRepairYard = true;
	}
}



void UnitClass::targeting()

{

	if (!target && !moving && !forced && (attackMode != SCOUT) && (findTargetTimer == 0) && (settings.playerType != CLIENT))

	{

		target = findTarget();

		if (target && isInAttackModeRange(target))

		{

			netDoAttack(target);

			pathList.clearList();

			nextSpotFound = false;

			speedCap = NONE;

		}

		else

		{

			target = NULL;

			if (attacking && (settings.playerType != CLIENT))

			{

				StructureClass* closestStructure = (StructureClass*)findClosestTargetStructure(this);

				if (closestStructure)

					netDoCommand(NULL, &closestStructure->getClosestPoint(&location), false);

				else

				{

					UnitClass* closestUnit = (UnitClass*)findClosestTargetUnit(this);

					if (closestUnit)

						netDoCommand(NULL, closestUnit->getLocation(), false);

				}

			}

		}

		findTargetTimer = 100;

	}

	

	if (target)

		engageTarget();

}



void UnitClass::turn()

{

	if (!moving)

	{

		int wantedAngle;

		if (target && (!targetFriendly || (targetDistance < 1.0)) && (targetDistance <= weaponRange))

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


void UnitClass::turnLeft()
{
	angle += turnSpeed;
	if (angle >= 7.5)
		angle -= 8.0;
	drawnAngle = round(angle);
}
void UnitClass::turnRight()
{
	angle -= turnSpeed;
	if (angle < -0.5)
		angle += 8;
	drawnAngle = round(angle);
}



void UnitClass::update()

{

	if (!destroyed)

	{

		if (active)

		{

			targeting();

			navigate();

			move();



			if (active)

				turn();

		}



		if (badlyDamaged)

		{

			if (health <= 0)

				netDestroy();

			else if (!goingToRepairYard

				&& owner->isAI()

				&& owner->hasRepairYard()

				&& !forced

				&& !target

				&& (settings.playerType != CLIENT))

			repair();

		}

	}



	if (smokeFrame != NONE)

	{

		if (++smokeFrame > 2)

			smokeFrame = 0;

	}

	if (frameTimer > 0)		//death frame has started

	{

		if (frameTimer == 1)

			frameChange();

		frameTimer--;

	}

	if (!destroyed)

	{

		if (checkTimer > 0) checkTimer--;

		if (findTargetTimer > 0) findTargetTimer--;

		if (primaryWeaponTimer > 0) primaryWeaponTimer--;

		if (secondaryWeaponTimer > 0) secondaryWeaponTimer--;

		if (deviationTimer > 0)

		{

			if (--deviationTimer == 0)

			{	//revert back to real owner

				removeFromSelectionLists();

				setTarget(NULL);

				setGuardPoint(&location);

				setDestination(&location);



				owner = realOwner;

				SDL_FreeSurface(graphic);
				graphic = (SDL_Surface*)(dataFile[baseID].dat);
				graphic =  copySurface(graphic);
				mapImageHouseColour(graphic, getOwner()->getColour());

			}

		}

	}

}



bool UnitClass::canPass(int xPos, int yPos)
{

	return (map->cellExists(xPos, yPos) && !map->getCell(xPos, yPos)->hasAGroundObject() && !map->getCell(xPos, yPos)->isMountain());
}



int UnitClass::getDrawnX()

{

	return ((int)(realX - imageW/2) - dborder->minX);

}



int UnitClass::getDrawnY()

{

	return ((int)(realY - imageH/2) - dborder->minY);

}



/* search algorithmns */
void UnitClass::nodePushSuccesors(PriorityQ* open, TerrainClass* parent_node)
{
	int dx1, dy1, dx2, dy2;
	double	cost,
			cross,
			heuristic,
			f;

	COORDTYPE	checkedPoint = destination,
				tempLocation;
	TerrainClass* node;

	//push a node for each direction we could go
	for (int angle=0; angle<=7; angle++)	//going from angle 0 to 7 inc
	{
		map->getMapPos(angle, &parent_node->location, &tempLocation);
		if (canPass(tempLocation.x, tempLocation.y))
		{
			node = &map->cell[tempLocation.x][tempLocation.y];
			cost = parent_node->cost;
			if ((location.x != parent_node->location.x) && (tempLocation.y != parent_node->location.y))
				cost += DIAGONALCOST*(isAFlyingUnit() ? 1.0 : (double)node->getDifficulty());	//add diagonal movement cost
			else
				cost += (isAFlyingUnit() ? 1.0 : (double)node->getDifficulty());
			/*if (parent_node->parent)	//add cost of turning time
			{
				int posAngle = map->getPosAngle(parent_node->parent->getLocation(), parent_node->getLocation());
				if (posAngle != angle)
					cost += (1.0/turnSpeed * (double)min(abs(angle - posAngle), NUM_ANGLES - max(angle, posAngle) + min(angle, posAngle)))/((double)BLOCKSIZE);
			}*/

			if (target)
				checkedPoint = target->getClosestPoint(&tempLocation);


			dx1 = tempLocation.x - checkedPoint.x;

			dy1 = tempLocation.y - checkedPoint.y;

			dx2 = location.x - checkedPoint.x;

			dy2 = location.y - checkedPoint.y;

			cross = (double)(dx1*dy2 - dx2*dy1);

			if( cross<0 ) cross = -cross;


			heuristic = blockDistance(&tempLocation, &checkedPoint);// + cross*0.1;//01;
			f = cost + heuristic;		
			if (node->visited)	//if we have already looked at this node before
				if (node->f <= f)	//if got here with shorter travel time before
					continue;

			TerrainClass* tempNode;
			if ((tempNode = open->findNodeWithKey(&tempLocation)))
			{
				if (tempNode->f <= f)
					continue;

				open->removeNodeWithKey(&tempLocation);
			}

			node->cost = cost;
			node->heuristic = heuristic;
			node->f = f;
			node->parent = parent_node;
			open->insertNode(node);
		}
	}
}

bool UnitClass::AStarSearch()
{
	int numNodesChecked = 0;
	COORDTYPE checkedPoint;
	


	TerrainClass *node = &map->cell[location.x][location.y];//initialise the current node the object is on
	if (target)
		checkedPoint = target->getClosestPoint(&location);
	else
		checkedPoint = destination;
	node->f = node->heuristic = blockDistance(&location, &checkedPoint);

	/*for (int i=0; i<max(map->sizeX, map->sizeY)-1; i++)
		if (map->depthCheckCount[i] != 0)	//very very bad if this happens, check if its in visited list and being reset to not visited
			selected = true;*/

	//if the unit is not directly next to its dest, or it is and the dest is unblocked
	if ((node->heuristic > 1.5) || canPass(checkedPoint.x, checkedPoint.y))
	{
		double smallestHeuristic = node->heuristic;
		PriorityQ open(map->sizeX*map->sizeY);
		LinkedList visitedList;
		TerrainClass	*bestDest = NULL; //if we dont find path to destination, we will head here instead


		node->next = node->parent = node->previous = NULL;
		node->cost = 0.0;
		open.insertNode(node);

		short	maxDepth = std::max(map->sizeX, map->sizeY),
				depth;

		while (!open.isEmpty())
		{
			//take the closest node to target out of the queue
			node = open.removeNode();


			if (node->heuristic < smallestHeuristic)
			{
				smallestHeuristic = node->heuristic;
				bestDest = node;

				if (node->heuristic == 0.0)	//if the distance from this node to dest is zero, ie this is the dest node

					break;	//ive found my dest!
			}

			if (numNodesChecked < settings.maxPathSearch)
				nodePushSuccesors(&open, node);
			if (!node->visited)
			{
				depth = std::max(abs(node->location.x - checkedPoint.x), abs(node->location.y - checkedPoint.y));
				if (++map->depthCheckCount[depth] >= map->depthCheckMax[checkedPoint.x][checkedPoint.y][depth])
					break;	//we have searched a whole square around destination, it cant be reached
				visitedList.insertFirst(node);	//so know which ones to reset to unvisited
				node->visited = true;
				numNodesChecked++;

				//if (debug)	//see all spots checked
				//	owner->placeUnit(Unit_Carryall, node->location.x, node->location.y);
			}
		}

		while (!visitedList.isEmpty())
		{
			node = (TerrainClass*)visitedList.removeFirst();
			node->visited = false;
			depth = std::max(abs(node->location.x - checkedPoint.x), abs(node->location.y - checkedPoint.y));
			map->depthCheckCount[depth] = 0;
		}


		//go to closest point to dest if is one
		if (bestDest != NULL)
		{

			node = bestDest;
			while (node->parent != NULL)
			{

				nextSpot = node->location;

				pathList.insertFirst(&node->location);

				//if (debug)	//see final path

				//getOwner()->placeUnit(Unit_Carryall, nextSpot.x, nextSpot.y);

				node = node->parent;
			}


			/*
			char temp[50];
			sprintf(temp, "%d at %d,%d to %d, %d: %d", itemID, location.x, location.y, destination.x, destination.y, numNodesChecked);
			showMessage(temp);*/
			return true;
		}
	}

	//no closer point found
	return false;
}

