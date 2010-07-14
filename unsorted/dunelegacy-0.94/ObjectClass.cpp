#include "sand.h"

#include "Items.h"

#define SMOKEDELAY 30

ObjectClass::ObjectClass(PlayerClass* newOwner)
{
	itemID = Unknown;
	objectID = nextObjectID++;

	aFlyingUnit = false;

	aGroundUnit = false;
	aStructure = false;

	aUnit = false;
	infantry = false;
	aBuilder = false;

	owner = newOwner;
	location.x = destination.x = NONE;

	location.y = destination.y = NONE;
	realX = realY = 0.0;
	drawnAngle = getRandomInt(0, 7);

	drawnFrame = 0;
	angle = 256/8*drawnAngle;

	active = false;
	badlyDamaged = false;
	canAttackStuff = false;
	destroyed = false;
	forced = false;

	selected = false;
	targetFriendly = false;

	setVisible(VIS_ALL, false);

	attackMode = AGGRESSIVE;


	counter = 0;

	animcounter = 0;

	animFrames = 1;

	animFrame = 0;

	isAnimating = 0;



	checkTimer = 0;
	frameTimer = -1;

	frameTime = 5;
	smokeFrame = NONE;
	weaponRange = 0;
	guardRange = DEFAULT_GUARDRANGE;

	armour = 5;
	radius = BLOCKSIZE/2;
	maxHealth = 100;
	health = 100;
///////////////////////////////////////////////////

	graphic = (SDL_Surface*)(dataFile[Unknown].dat);
	imageW = graphic->w;
	imageH = graphic->h;



	deathFrame = Object_SmallUnitExplosion;

	numDeathFrames = 2;



	numConfirmSounds = numDestroySounds = numSelectSounds = 0;

	setTarget(NULL);

	interfaceContainer = NULL;
}
ObjectClass::~ObjectClass()
{
}


void ObjectClass::assignToMap(COORDTYPE* pos)

{

	if (map->cellExists(pos->x, pos->y))

	{

		map->cell[pos->x][pos->y].assignNonInfantryGroundObject(this);

		map->viewMap(owner->getTeam(), &location, viewRange);

	}

}



void ObjectClass::blitToScreen()

{

	SDL_Rect dest, source;

	dest.x = getDrawnX();

	dest.y = getDrawnY();

	dest.w = source.w = imageW;

	dest.h = source.h = imageH;

	source.x = drawnAngle*imageW;

	source.y = owner->getHouse()*imageH;



	SDL_BlitSurface(graphic, &source, screen, &dest);

}



void ObjectClass::drawInterface()

{

	if (interfaceContainer != NULL)

		interfaceContainer->draw();

}





void ObjectClass::drawSmoke(int x, int y)

{

	int imageW;

	SDL_Rect dest, source;

	SDL_Surface* smoke = (SDL_Surface*)dataFile[Object_Smoke].dat;
	
	imageW = smoke->w/3;

	dest.x = x - imageW/2;

	dest.y = y - smoke->h;

	dest.w = imageW;

	dest.h = smoke->h;

	source.x = smokeLast;

	if(++smokeCounter >=SMOKEDELAY)
		{
			smokeCounter = 0;
		    source.x = imageW*getRandomInt(0, 2);
			smokeLast = source.x;
		};

	source.y = 0;

	source.w = imageW;

	source.h = smoke->h;



	SDL_BlitSurface(smoke, &source, screen, &dest);

}



void ObjectClass::getClosestCentrePoint(COORDTYPE* centrePoint, COORDTYPE* objectLocation)

{

	getCentrePoint(centrePoint);

}



void ObjectClass::getCentrePoint(COORDTYPE* centrePoint)

{

	centrePoint->x = round(realX);

	centrePoint->y = round(realY);

}



void ObjectClass::handleDamage(int damage, ObjectClass* damager)
{
	if (!wasDestroyed())
	{
		if (damage >= 0)
		{
			health -= damage;
			if (health < 0)
				health = 0;
			if (!badlyDamaged && (health/(double)maxHealth < HEAVILYDAMAGEDRATIO))
				badlyDamaged = true;
		}

		if (getOwner() == thisPlayer)
		{
			/*
			if(!(dynamic_cast<StructureClass*>(this) == NULL))
			{
				switch(getOwner()->getHouse())
				{
					case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
						soundPlayer->playVOSound(SoundVO_Aattack);
						break;
					case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
						soundPlayer->playVOSound(SoundVO_Oattack);
						break;
					case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
						soundPlayer->playVOSound(SoundVO_Hattack);
						break;
					default:
						break;
				}
			}*/
			soundPlayer->changeMusic(MUSIC_ATTACK);
		}
		getOwner()->noteDamageLocation(this, &location);
	}
}



void ObjectClass::handleInterfaceEvent(SDL_Event* event)
{
}



void ObjectClass::netDestroy()
{
	if (settings.playerType != CLIENT)
	{
		destroy();
		if (settings.playerType == SERVER)

			netServerDestroyObject(objectID, location.y, location.y);
	}
	else
		netClientDestroyObject(objectID, location.y, location.y);
}



void ObjectClass::netDoAttack(ObjectClass* newTarget)

{

	if (settings.playerType != SINGLE)
	{
		setTarget(newTarget);
		if (settings.playerType == SERVER)
			netServerAttackMessage(objectID, newTarget->getObjectID(), location.x, location.y, newTarget->getX(), newTarget->getY());
	}
}



void ObjectClass::netDoCommand(ObjectClass* newTarget, COORDTYPE* dest, bool forced)
{
	if (settings.playerType == SINGLE)
	{
		setTarget(newTarget);
		setDestination(dest);
		setForced(forced);
	}
	else if (settings.playerType == SERVER)
	{
		setTarget(newTarget);
		setDestination(dest);
		setForced(forced);
		netServerObjectMessage(objectID, newTarget ? newTarget->getObjectID() : NONE, &location, dest, forced);
	}
	else
		netClientObjectMessage(objectID, newTarget ? newTarget->getObjectID() : NONE, &location, dest, forced);
}



void ObjectClass::playConfirmSound()

{

	if (numConfirmSounds)

		soundPlayer->playResponseSound(confirmSound[getRandomInt(0, numConfirmSounds-1)]);

}



void ObjectClass::playSelectSound()

{

	if (numSelectSounds)

		soundPlayer->playResponseSound(selectSound[getRandomInt(0, numSelectSounds-1)]);

}



void ObjectClass::removeFromSelectionLists()

{

	ObjectClass *tempObject;



	selectedList->reset();

	while(selected && selectedList->currentNotNull())

	{

		tempObject = (ObjectClass*)selectedList->getCurrent();



		if (tempObject == this)

		{

			selectedList->removeCurrent();

			selected = false;

		}

		else

			selectedList->nextLink();

	}



	ListIterator* tempList;

	for (int listCount = 0; listCount < 9; listCount++)

	{

		tempList = &selectedLists[listCount];



	    tempList->reset();

	    while(tempList->currentNotNull())

	    {

	        tempObject = (ObjectClass*)tempList->getCurrent();



        	if (tempObject == this)

                tempList->removeCurrent();

	        else tempList->nextLink();

	    }

	}

}



void ObjectClass::setDestination(int newX, int newY)
{
	if (map->cellExists(newX, newY) || ((newX == NONE) && (newY == NONE)))
	{
		destination.x = newX;
		destination.y = newY;
	}
}



void ObjectClass::setHealth(int newHealth)

{

	if ((newHealth >= 0) && (newHealth <= maxHealth))

	{

		health = newHealth;

		if (!badlyDamaged && (health/(double)maxHealth < HEAVILYDAMAGEDRATIO))
			badlyDamaged = true;

	}

}



void ObjectClass::setLocation(int xPos, int yPos)

{

	if (map->cellExists(xPos, yPos) || ((xPos == NONE) && (yPos == NONE)))

	{

		location.x = xPos;

		location.y = yPos;

		realX = location.x*BLOCKSIZE;

		realY = location.y*BLOCKSIZE;



		assignToMap(&location);

	}

}

void ObjectClass::setObjectID(int newObjectID)

{
	if (newObjectID >= 0)
		objectID = newObjectID;
}

void ObjectClass::setVisible(int team, bool status)
{
	if (team == VIS_ALL)
	{
		for(int i = 0; i < MAX_PLAYERS; i++)
			visible[i] = status;
	}
	else if ((team >= 1) && (team <= MAX_PLAYERS))
		visible[--team] = status;
}

void ObjectClass::setTarget(ObjectClass* newTarget)
{
	target = newTarget;

	targetFriendly = (target && (target->getOwner()->getTeam() == owner->getTeam()) && (target->getItemID() != Unit_Sandworm));
}



void ObjectClass::unassignFromMap(COORDTYPE* location)

{

	if (map->cellExists(location))

		map->getCell(location)->unassignObject(this);

}



bool ObjectClass::canAttack(ObjectClass* object)

{

	if (!object->wasDestroyed()

		&& (object->isAStructure()

			|| !object->isAFlyingUnit())

		&& ((object->getOwner()->getTeam() != owner->getTeam())

			|| object->getItemID() == Unit_Sandworm)

		&& object->isVisible(getOwner()->getTeam()))

		return true;

	return false;

}



bool ObjectClass::isOnScreen()

{

	if (((getRealX() + getImageW()/2) >= dborder->minX) && ((getRealX() - getImageW()/2) <= dborder->maxX+1) && ((getRealY() + getImageH()/2) >= dborder->minY) && ((getRealY() - getImageH()/2)<= dborder->maxY+1))

		return true;

	else

		return false;

}


bool ObjectClass::isVisible(int team)

{

	if ((team >= 1) && (team <= MAX_PLAYERS))

		return visible[team-1];

	return false;

}


int ObjectClass::getHealthColour()
{
	int healthColour = COLOUR_RED;
	double healthPercent = (double)health/(double)maxHealth;

	if (healthPercent >= 0.7)
		healthColour = COLOUR_LIGHTGREEN;
	else if (healthPercent >= HEAVILYDAMAGEDRATIO)
		healthColour = COLOUR_YELLOW;

	return healthColour;
}



COORDTYPE ObjectClass::getClosestPoint(COORDTYPE* point)

{

	return location;

}

ObjectClass* ObjectClass::findClosestTargetStructure(ObjectClass* object)

{

	StructureClass	*closestStructure = NULL,
					*tempStructure;
	double			closestDistance = 100000000.0,
					structureDistance;

	if (!structureList->isEmpty())
	{
		structureList->saveCurrent();
		structureList->reset();
		while(structureList->currentNotNull())
		{
			tempStructure = ((StructureClass*)structureList->getCurrent());

			if (object->canAttack(tempStructure))
			{
				structureDistance = blockDistance(object->getLocation(), &tempStructure->getClosestPoint(object->getLocation()));

				if(tempStructure->getItemID() == Structure_Wall)
				{
					structureDistance += 20000000.0;//so that walls are targeted very last
				}

				if (structureDistance < closestDistance)
				{
					closestDistance = structureDistance;
					closestStructure = tempStructure;
				}
			}
			structureList->nextLink();
		}
		structureList->restoreCurrent();
	}

	return closestStructure;
}



ObjectClass* ObjectClass::findClosestTargetUnit(ObjectClass* object)
{
	UnitClass	*closestUnit = NULL,
				*tempUnit;
	double		closestDistance = 100000000.0,
				unitDistance;

	if (!unitList->isEmpty())
	{
		unitList->saveCurrent();
		unitList->reset();
		while(unitList->currentNotNull())
		{
			tempUnit = ((UnitClass*)unitList->getCurrent());

			if (object->canAttack(tempUnit))
			{
				unitDistance = blockDistance(object->getLocation(), &tempUnit->getClosestPoint(object->getLocation()));

				if (unitDistance < closestDistance)
				{
					closestDistance = unitDistance;
					closestUnit = tempUnit;
				}
			}
			unitList->nextLink();
		}
		unitList->restoreCurrent();
	}

	return closestUnit;
}


ObjectClass* ObjectClass::findTarget()

{

	ObjectClass	*tempTarget,

				*closestTarget = NULL;



	int	checkRange,

		xPos = location.x,

		yPos = location.y;



	double closestDistance = 1000000.0;



//searches for a target in an area like as shown below 



//                     *****

//                   *********

//                  *****T*****

//                   *********

//                     *****



	if (attackMode == STANDGROUND)

		checkRange = weaponRange;

	else

		checkRange = guardRange;



	int xCheck = xPos - checkRange;

	if (xCheck < 0) xCheck = 0;

	int yCheck;



	while ((xCheck < map->sizeX) && ((xCheck - xPos) <=  checkRange))

	{

		yCheck = (yPos - lookDist[abs(xCheck - xPos)]);

		if (yCheck < 0) yCheck = 0;



		while ((yCheck < map->sizeY) && ((yCheck - yPos) <=  lookDist[abs(xCheck - xPos)]))

		{

			if (map->cell[xCheck][yCheck].hasAnObject())

			{

				tempTarget = map->cell[xCheck][yCheck].getObject();

				if (((tempTarget->getItemID() != Structure_Wall) || (closestTarget == NULL)) && canAttack(tempTarget))

				{

					double targetDistance = blockDistance(&location, tempTarget->getLocation());

					if (targetDistance < closestDistance)

					{

						closestTarget = tempTarget;

						closestDistance = targetDistance;

					}

				}

			}



			yCheck++;

		}



		xCheck++;

		yCheck = yPos;

	}



	return closestTarget;

}

void ObjectClass::clearObject()
{// { return (destroyed && (frameTimer == 0)); }
	destroyed = true;
	frameTimer = 0;
}

/*inline*/ 
int ObjectClass::getViewRange()
{
  if (owner->hasRadarOn() )
	  return viewRange+2;
	else
	  return viewRange;
};

