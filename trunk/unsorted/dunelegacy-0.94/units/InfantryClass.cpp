#include "../sand.h"

#include "../Items.h"



InfantryClass::InfantryClass(PlayerClass* newOwner) : GroundUnit(newOwner)

{

	baseID = itemID = Unit_Infantry;



	infantry = true;



	cellPosition = oldCellPosition = 0;

	deathFrame = Object_DeadInfantry;

	numDeathFrames = 2;

	walkFrame = 0;



	armour = 1;

	frameTime = 500;

	maxHealth = INFANTRY_HITPOINTS;

	health = maxHealth;

	radius = 2;

	speed = 0.25;

	viewRange = INFANTRY_VIEWRANGE;

	weaponRange = INFANTRY_WEAPONRANGE;



	numWeapons = 1;	

	bulletType[0] = Bullet_Inf_Gun;

	primaryWeaponReloadTime = 55;



	graphic = (SDL_Surface*)(dataFile[baseID].dat);

	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());

	imageW = graphic->w/4;

	imageH = graphic->h/3;

	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place

	yOffset = (imageH - BLOCKSIZE)/2;



	squashed = false;



	numAttackSounds = 1;

	numConfirmSounds = 2;

	numDestroySounds = 5;

	numSelectSounds = 1;



	attackSound[0] = Sound_gun;

	confirmSound[0] = Sound_infantryOut;

	confirmSound[1] = Sound_movingOut;

	destroySound[0] = Sound_scream1;

	destroySound[1] = Sound_scream2;

	destroySound[2] = Sound_scream3;

	destroySound[3] = Sound_scream4;

	destroySound[4] = Sound_scream5;

	selectSound[0] = Sound_yesSir;

}

InfantryClass::~InfantryClass()

{

	if (destroyed)

		SDL_FreeSurface(graphic);

}



void InfantryClass::assignToMap(COORDTYPE* pos)

{

	if (map->cellExists(pos->x, pos->y))

	{

		oldCellPosition = cellPosition;

		cellPosition = map->cell[pos->x][pos->y].assignInfantry(this, cellPosition);

	}

}



void InfantryClass::blitToScreen()

{

	SDL_Rect dest, source;

	if (!destroyed)

	{

		dest.x = getDrawnX();

		dest.y = getDrawnY();

		dest.w = source.w = imageW;

		dest.h = source.h = imageH;



		int temp = drawnAngle;

		if (temp == UP)

			temp = 1;

		else if (temp == DOWN)

			temp = 3;

		else if (temp == LEFTUP || temp == LEFTDOWN || temp == LEFT)

			temp = 2;

		else	//RIGHT

			temp = 0;

		source.x = temp*imageW;

		if (walkFrame/10 == 3)

			source.y = imageH;

		else

			source.y = walkFrame/10*imageH;



		SDL_BlitSurface(graphic, &source, screen, &dest);



		if (selected)

			selectionBlitList->insertLast(this);

	}

	else

	{

		imageW = graphic->w/2;

		imageH = graphic->w/2;

		xOffset = (imageW - BLOCKSIZE)/2;		    //this is where it actually draws the graphic

		yOffset = (imageH - BLOCKSIZE)/2;

		dest.x = getDrawnX();

		dest.y = getDrawnY();

		dest.w = source.w = imageW;

		dest.h = source.h = imageH;

		if (squashed)

		{

			source.x = imageW*drawnFrame;

			source.y = imageH;

		}

		else

			source.x = source.y = 0;



		SDL_BlitSurface(graphic, &source, screen, &dest);

	}

}



bool InfantryClass::canPass(int xPos, int yPos)

{

	bool passable = false;

	if (map->cellExists(xPos, yPos))

	{

		TerrainClass* cell = map->getCell(xPos, yPos);

		if (!cell->hasAGroundObject())

		{

			if (cell->getType() != Terrain_Mountain)

				passable = true;

			else

			{

				/* if this unit is infantry so can climb, and cell can take more infantry */

				if (cell->infantryNotFull())

					passable = true;

			}

		}

		else

		{

			ObjectClass *object = cell->getGroundObject();

			

			if (object == target && !object->wasDestroyed() && object->isAStructure() && (object->getOwner()->getTeam() != owner->getTeam()) && object->isVisible(getOwner()->getTeam()))

				passable = true;

			else

				passable = (!cell->hasANonInfantryGroundObject()

				&& (cell->infantryNotFull()

					&& (cell->getInfantryTeam() == getOwner()->getTeam())));

		}

	}



	return passable;

}



void InfantryClass::checkPos()

{

	if (moving)

	{

		if (++walkFrame > 39)

			walkFrame = 0;

	}



	if (justStoppedMoving)

	{

		walkFrame = 0;



		if (map->getCell(&location)->isBloom())

		{

			COORDTYPE realPos;



			realPos.x = location.x*BLOCKSIZE + BLOCKSIZE/2;

			realPos.y = location.y*BLOCKSIZE + BLOCKSIZE/2;

			map->getCell(&location)->damageCell(this, getOwner(), &realPos, NONE, 500, NONE, BLOCKSIZE, false);

		}



		/*

			add check if unit is on structure, and damage/takeover

		*/

		if (justStoppedMoving)

		{	//check to osee if close enough to blow up target

			if (target && target->isAStructure() && (getOwner()->getTeam() != target->getOwner()->getTeam()))

			{

				COORDTYPE	closestPoint;



				closestPoint = target->getClosestPoint(&location);

		        

				

				if (blockDistance(&location, &closestPoint) <= 0.5)	

				{

					if(target->getHealthColour() == COLOUR_RED)

					{

						

						int targetID = target->getItemID();

						int targetOID = target->getObjectID();

						int posX = target->getX();

						int posY = target->getY();

						int h;

						int oldHealth = target->getHealth();



						

						//StructureClass *temp = (StructureClass *)structureList->removeElement(target);

						

						StructureClass *temp = (StructureClass *)target;

						temp->setHealth(0);

						h = temp->getOriginalHouse();

						//temp->clearObject();



						if(temp)

						{

							//delete temp;

							temp = (StructureClass*)owner->placeStructure(NONE, NONE, NONE, targetID, posX, posY);

							temp->setObjectID(targetOID);

							temp->setOriginalHouse(h);

							temp->setHealth(oldHealth);

							setTarget(NULL);

						}

						

					}

					else

					{

						COORDTYPE realPos;



						realPos.x = location.x*BLOCKSIZE + BLOCKSIZE/2;

						realPos.y = location.y*BLOCKSIZE + BLOCKSIZE/2;

						map->getCell(&location)->damageCell(this, getOwner(), &realPos, NONE, 100, NONE, BLOCKSIZE, false);

					}

					netDestroy();

				}

			}

			else if(target && target->isAStructure())

			{

				COORDTYPE	closestPoint;



				closestPoint = target->getClosestPoint(&location);

		        

				

				if (blockDistance(&location, &closestPoint) <= 0.5)	

				{

					netDestroy();

				}

			}

		}



		//if (settings.playerType == SERVER)

			//netDoCommand(target, &destination, forced);

	}



	if (goingToRepairYard)

	{

		if ((blockDistance(&location, &target->getClosestPoint(&location)) <= 1.5) && ((RepairYardClass*)target)->isFree())

		{

			if (health*100/maxHealth < 100)

				setGettingRepaired();

			else

			{

				setTarget(NULL);

				setDestination(&guardPoint);

			}

		}

	}

}



void InfantryClass::destroy()

{

	UnitClass::destroy();

	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());

}



void InfantryClass::move()

{
	if(!moving && getRandomInt(0,40) == 0)
		map->viewMap(owner->getTeam(), &location, viewRange); 
	
	if (moving)

	{

		COORDTYPE	oldLocation = location,

					wantedReal;



		realX += xSpeed;

		realY += ySpeed;



		wantedReal.x = nextSpot.x*BLOCKSIZE + BLOCKSIZE/2,

		wantedReal.y = nextSpot.y*BLOCKSIZE + BLOCKSIZE/2;



		switch (cellPosition)

		{

		case 1:

			wantedReal.x -= BLOCKSIZE/4;

			wantedReal.y -= BLOCKSIZE/4;

			break;

		case 2:

			wantedReal.x += BLOCKSIZE/4;

			wantedReal.y -= BLOCKSIZE/4;

			break;

		case 3:

			wantedReal.x -= BLOCKSIZE/4;

			wantedReal.y += BLOCKSIZE/4;

			break;

		case 4:

			wantedReal.x += BLOCKSIZE/4;

			wantedReal.y += BLOCKSIZE/4;

			break;

		default:

			break;

		}



		if ((fabs((double)wantedReal.x - realX) <= 0.2) && (fabs((double)wantedReal.y - realY) <= 0.2))

		{

			realX = wantedReal.x;

			location.x = nextSpot.x;

			realY = wantedReal.y;

			location.y = nextSpot.y;



			if ((location.x == destination.x) && (location.y == destination.y))

				setForced(false);



			moving = false;

			justStoppedMoving = true;



			unassignFromMap(&oldLocation);



			map->viewMap(owner->getTeam(), &location, viewRange);

		}

	}

	else

		justStoppedMoving = false;



	checkPos();

}



void InfantryClass::setLocation(int xPos, int yPos)

{

	if (map->cellExists(xPos, yPos) || ((xPos == NONE) && (yPos == NONE)))

	{

		oldCellPosition = cellPosition = 0;

		GroundUnit::setLocation(xPos, yPos);

		switch (cellPosition)

		{

		case 1:

			realX -= BLOCKSIZE/4;

			realY -= BLOCKSIZE/4;

			break;

		case 2:

			realX += BLOCKSIZE/4;

			realY -= BLOCKSIZE/4;

			break;

		case 3:

			realX -= BLOCKSIZE/4;

			realY += BLOCKSIZE/4;

			break;

		case 4:

			realX += BLOCKSIZE/4;

			realY += BLOCKSIZE/4;

			break;

		default:

			break;

		}

	}

}



void InfantryClass::setSpeeds()

{

	if (oldCellPosition == NONE)

		fprintf(stderr, "ERROR: infantry cell position NONE.");

	else if (cellPosition == oldCellPosition)	//havent changed infantry position

		GroundUnit::setSpeeds();

	else

	{

		int sx, sy, dx, dy;

		switch (oldCellPosition)

		{

		case 0: sx = sy = 0; break;

		case 1: sx = -BLOCKSIZE/4; sy = -BLOCKSIZE/4; break;

		case 2: sx = BLOCKSIZE/4; sy = -BLOCKSIZE/4; break;

		case 3: sx = -BLOCKSIZE/4; sy = BLOCKSIZE/4; break;

		case 4: sx = BLOCKSIZE/4; sy = BLOCKSIZE/4; break;

		}



		switch (cellPosition)

		{

		case 0: dx = dy = 0; break;

		case 1: dx = -BLOCKSIZE/4; dy = -BLOCKSIZE/4; break;

		case 2: dx = BLOCKSIZE/4; dy = -BLOCKSIZE/4; break;

		case 3: dx = -BLOCKSIZE/4; dy = BLOCKSIZE/4; break;

		case 4: dx = BLOCKSIZE/4; dy = BLOCKSIZE/4; break;

		}

		

		switch (drawnAngle)

		{

		case (RIGHT): dx += BLOCKSIZE; break;

		case (RIGHTUP): dx += BLOCKSIZE; dy -= BLOCKSIZE; break;

		case (UP): dy -= BLOCKSIZE; break;

		case (LEFTUP): dx -= BLOCKSIZE; dy -= BLOCKSIZE; break;

		case (LEFT): dx -= BLOCKSIZE; break;

		case (LEFTDOWN): dx -= BLOCKSIZE; dy += BLOCKSIZE; break;

		case (DOWN): dy += BLOCKSIZE; break;

		case (RIGHTDOWN): dx += BLOCKSIZE; dy += BLOCKSIZE; break;

		}



		dx -= sx;

		dy -= sy;



		double scale = speed/sqrt((double)(dx*dx + dy*dy));

		xSpeed = dx*scale;

		ySpeed = dy*scale;

	}

}



void InfantryClass::squash()

{

	netDestroy();

	drawnFrame = getRandomInt(0, 1);

	squashed = true;

}



bool InfantryClass::canAttack(ObjectClass* object)

{

	if (!object->wasDestroyed()

		&& (object->isAStructure()

			|| !object->isAFlyingUnit())

		&& ((object->getOwner()->getTeam() != owner->getTeam())

			|| object->getItemID() == Unit_Sandworm)

		&& object->isVisible(getOwner()->getTeam()))

	{

		if(object->isAStructure() && object->getItemID() != Structure_Wall && object->getHealthColour() == COLOUR_RED)

			weaponRange = 0;

		else

			weaponRange = INFANTRY_WEAPONRANGE;



		return true;

	}

	else

		return false;

}

