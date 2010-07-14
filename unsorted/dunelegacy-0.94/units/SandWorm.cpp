#include "../sand.h"

#include "../Items.h"



#define MAX_SANDWORMSLEEPTIME 50000

#define MIN_SANDWORMSLEEPTIME 10000



Sandworm::Sandworm(PlayerClass* newOwner) : GroundUnit(newOwner)

{

	baseID = itemID = Unit_Sandworm;



	armour = 5;

	maxHealth = SANDWORM_HITPOINTS;

	health = maxHealth;

	speed = 0.3;

	turnSpeed = 0.625;

	radius = 8;

	viewRange = SANDWORM_VIEWRANGE;

	weaponRange = SANDWORM_WEAPONRANGE;

	

	numWeapons = 0;

	primaryWeaponReloadTime = 200;



	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	//mapImageHouseColour(graphic, getOwner()->getColour());

	imageW = graphic->w;

	imageH = graphic->h/5;

	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place

	yOffset = (imageH - BLOCKSIZE)/2;

	drawnFrame = NONE;



	attackMode = DEFENSIVE;



	numAttackFrames = 5;

	frameTime = 15;

	deathFrame = Unknown;

	numDeathFrames = 1;



	kills = 0;

	sleepTimer = 0;



	numAttackSounds = 1;

	attackSound[0] = Sound_wormAttack;



	shimmerSurface = new SDL_Surface*[SANDWORM_SEGMENTS];

	for(int i = 0; i < SANDWORM_SEGMENTS; i++)

	{

		shimmerSurface[i] = copySurface((SDL_Surface*)dataFile[Object_ShimmerMask].dat);

		SDL_FillRect(shimmerSurface[i], NULL, COLOUR_CLEAR);

	}

}



Sandworm::~Sandworm()

{

	for(int i = 0; i < SANDWORM_SEGMENTS; i++)

		SDL_FreeSurface(shimmerSurface[i]);

	delete[] shimmerSurface;

}



void Sandworm::assignToMap(COORDTYPE* pos)

{

	if (map->cellExists(pos->x, pos->y))

	{

		map->cell[pos->x][pos->y].assignUndergroundUnit(this);

		map->viewMap(owner->getTeam(), &location, viewRange);

	}

}



void Sandworm::attack()

{

	if (primaryWeaponTimer == 0)

	{

		if (target)

		{

			soundPlayer->playSoundAt(Sound_wormAttack, &location);

			drawnFrame = 0;

			frameTimer = frameTime;

			primaryWeaponTimer = primaryWeaponReloadTime;

		}

	}

}



void Sandworm::blitToScreen()

{

	if (!destroyed)

	{

		SDL_Rect dest, source;



		dest.w = source.w = shimmerSurface[0]->w;

		dest.h = source.h = shimmerSurface[0]->h;

		source.x = 0;

		source.y = 0;



		if (moving)

		{	//create worms shimmer

			if (shimmerSurface[0]->format->BitsPerPixel == 8)

			{

				/*TerrainClass	*cell;

				SDL_Surface		*cellSurface = NULL,

								*lastCellSurface = NULL;*/

				SDL_Surface *mask = (SDL_Surface*)dataFile[Object_ShimmerMask].dat;



				if ((!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) >= 0))

					&& (!SDL_MUSTLOCK(mask) || (SDL_LockSurface(mask) >= 0)))

				{

					unsigned char	*maskPixels = (unsigned char*)mask->pixels,

									*screenPixels = (unsigned char*)screen->pixels,

									*surfacePixels;

					int count, i,j, x,y,
						 maxX = fullscreenMode ? screen->w : gameBarPos.x;

					for(count = 0; count < SANDWORM_SEGMENTS; count++)	//for each segment of the worms length

					{

						if ((!SDL_MUSTLOCK(shimmerSurface[count]) || (SDL_LockSurface(shimmerSurface[count]) >= 0)))

						{

							surfacePixels = (unsigned char*)shimmerSurface[count]->pixels;



						/*	char temp[30];

							sprintf(temp, "screens data:%d, surface data:%d", screen->format->BitsPerPixel, shimmerSurface[count]->format->BitsPerPixel);

							showMessage(temp);*/



							dest.x = lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].x - dborder->minX - dest.w/2;

							dest.y = lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].y - dborder->minY - dest.h/2;



							for (i = 0; i < dest.w; i++)

							for (j = 0; j < dest.h; j++)

							{

								//lastCellSurface = cellSurface;

								//surfacePixels[i + j*dest.w] = getRandomInt(89, 93);

								if (maskPixels[i + j*mask->pitch] == 0)	//direct copy

								{

									x = i;

									y = j;

								}

								else

								{

									x = i + getRandomInt(-3, 3);

									y = j + getRandomInt(-3, 3);

								}



								if (dest.x + x < 0)

									dest.x = x = 0;

								else if (dest.x + x >= maxX)

									dest.x = maxX - 1, x = 0;

								if (dest.y + y < 0)

									dest.y = y = 0;

								else if (dest.y + y >= screen->h)

									dest.y = screen->h - 1, y = 0;

/*

								cell = map->getCell((dest.x + x + dborder->minX)/BLOCKSIZE, (dest.y + y + dborder->minY)/BLOCKSIZE);

								cellSurface = cell->getSprite();

								if (cellSurface != lastCellSurface)

								{

									if ((lastCellSurface != NULL) && SDL_MUSTLOCK(lastCellSurface))

										SDL_UnlockSurface(lastCellSurface);

									if (SDL_MUSTLOCK(cellSurface) && (SDL_LockSurface(cellSurface) < 0))

									{

										count = SANDWORM_SEGMENTS;

										i = dest.w;

										j = dest.h;

										cellSurface = NULL;

										break;

									}

								}*/



								//surfacePixels[i + j*dest.w] = ((unsigned char*)cellSurface->pixels)[cell->getTile()*BLOCKSIZE + (dest.x + x + dborder->minX)%BLOCKSIZE + ((dest.y + y + dborder->minY)%BLOCKSIZE)*cellSurface->w];

								surfacePixels[i + j*shimmerSurface[count]->pitch] = screenPixels[dest.x + x + (dest.y + y)*screen->pitch];

								//fprintf(stdout, "%d,", surfacePixels[i + j*dest.w]);

							}



							//if (cellSurface && SDL_MUSTLOCK(cellSurface))

							//	SDL_UnlockSurface(cellSurface);

							if (SDL_MUSTLOCK(shimmerSurface[count]))

								SDL_UnlockSurface(shimmerSurface[count]);

						}

					}



					if (SDL_MUSTLOCK(mask))

						SDL_UnlockSurface(mask);

					if (SDL_MUSTLOCK(screen))

						SDL_UnlockSurface(screen);

				}

			}

		}



		/////draw wormy shimmer segments

		for(int count = 0; count < SANDWORM_SEGMENTS; count++)	//draw all the shimmering images

		{

			dest.x = lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].x - dborder->minX - dest.w/2;

			dest.y = lastLocs[count*(SANDWORM_LENGTH/SANDWORM_SEGMENTS)].y - dborder->minY - dest.h/2;



			SDL_BlitSurface(shimmerSurface[count], &source, screen, &dest);

		}



		if (drawnFrame != NONE) //if attacking

		{

			dest.x = getDrawnX();

			dest.y = getDrawnY();

			dest.w = source.w = imageW;

			dest.h = source.h = imageH;

			source.x = 0;

			source.y = drawnFrame*imageH;



			SDL_BlitSurface(graphic, &source, screen, &dest);

		}



		if (selected)

			selectionBlitList->insertLast(this);

	}

}



void Sandworm::checkPos()

{

	if (moving)

	{

		if ((abs((short)realX - lastLocs[0].x) >= 1)

			|| (abs((short)realY - lastLocs[0].y) >= 1))

		{

			for (int i = (SANDWORM_LENGTH-1); i > 0 ; i--)

				lastLocs[i] = lastLocs[i-1];

			lastLocs[0].x = (short)realX;

			lastLocs[0].y = (short)realY;

		}

	}



	if (justStoppedMoving)

	{

		realX = location.x*BLOCKSIZE + BLOCKSIZE/2;

		realY = location.y*BLOCKSIZE + BLOCKSIZE/2;



		if (settings.playerType == SERVER)
			netDoCommand(target, &destination, forced);

		

		if (map->cellExists(&location))

		{

			TerrainClass* cell = map->getCell(&location);

			if (cell->hasInfantry() && (cell->getInfantry()->getOwner() == thisPlayer))

				soundPlayer->playResponseSound(Sound_something);

		}

	}

}



void Sandworm::engageTarget()

{

	if (target)

	{

		if (!canAttack(target) || (!forced && !isInAttackModeRange(target)))

		{

			forced = false;

			setTarget(NULL);



			setGuardPoint(&location);	//so will stop at current location, remove this line to go to

			setDestination(&guardPoint);	//old way of destroyed units location



			pathList.clearList();

		}

		else

		{

			COORDTYPE targetLocation = target->getClosestPoint(&location);

			targetDistance = blockDistance(&location, &targetLocation);



			if ((destination.x != targetLocation.x)	//targets location has moved, recalculate path

				&& (destination.y != targetLocation.y))

				pathList.clearList();



			if (forced)

				setDestination(&targetLocation);

			else

			{

				switch (attackMode)

				{

				case (DEFENSIVE): //if the target is not further then twice the guard range plus weapon range, and this unit is not further then twice the guardrange away, set it as target

					if ((blockDistance(&guardPoint, &targetLocation) + targetDistance) > (2*guardRange))

					{

						setDestination(&guardPoint);

						setTarget(NULL);

					}

					else

						setDestination(&targetLocation);

					break;

				case (AGGRESSIVE): default://don't care go after enemy unit

					setDestination(&targetLocation);

					break;

				}

			}



			if (target)

			{

				targetAngle = round(8.0/256.0*dest_angle(&location, &targetLocation));

				if (targetAngle == 8)

					targetAngle = 0;



				if (targetDistance <= weaponRange)

				{

					setDestination(&location);

					nextSpotFound = false;

					attack();

				}

			}

		}

	}

}



void Sandworm::frameChange()

{

	if (destroyed)

		UnitClass::frameChange();

	else

	{

		drawnFrame++;

		if (drawnFrame < numAttackFrames)

		{

			frameTimer = frameTime;

			if (drawnFrame == 1)	//the close mouth bit of graphic

			{

				bool wasAlive = ((target != NULL) && target->isVisible(getOwner()->getTeam()));	//see if was alive before attack

				COORDTYPE realPos = { (short)realX, (short)realY };

				map->getCell(&location)->damageCell(this, getOwner(), &realPos, itemID, 5000, NONE, NONE, false);

				if (wasAlive && !target->isVisible(getOwner()->getTeam()))

					kills++;			

			}

		}

		else

		{

			drawnFrame = NONE;

			if (kills >= 3)

				sleep();

		}

	}

}



void Sandworm::setLocation(int xPos, int yPos)

{

	if (map->cellExists(xPos, yPos) || ((xPos == NONE) && (yPos == NONE)))

	{

		UnitClass::setLocation(xPos, yPos);

		for(int i = 0; i < SANDWORM_LENGTH; i++)

		{

			lastLocs[i].x = (short)realX;

			lastLocs[i].y = (short)realY;

		}

	}

}



void Sandworm::setSleepTimer(int newSleepTime)

{

	if ((newSleepTime >= MIN_SANDWORMSLEEPTIME) && (newSleepTime <= MAX_SANDWORMSLEEPTIME))

		sleepTimer = newSleepTime;

}



void Sandworm::sleep()

{	//put sandworm to sleep for a while

	sleepTimer = getRandomInt(MIN_SANDWORMSLEEPTIME, MAX_SANDWORMSLEEPTIME);

	setActive(false);

	setVisible(VIS_ALL, false);

	map->removeObjectFromMap(this);	//no map point will reference now

	setLocation(NONE, NONE);

	health = maxHealth;

	kills = 0;

	drawnFrame = NONE;

	frameTimer = 0;

}



void Sandworm::update()

{

	if (health <= maxHealth/2)

		sleep();

	else

	{

		GroundUnit::update();

		if (sleepTimer > 0)

		{

			if (--sleepTimer <= 0)

			{	//awaken the worm!

				int i, j;



				do

				{

					i = getRandomInt(0, map->sizeX - 1);

					j = getRandomInt(0, map->sizeY - 1);

					sleepTimer++;

				} while (!canPass(i, j) && (sleepTimer < 1000));



				if (canPass(i, j))

				{

					deploy(map->getCell(i, j)->getLocation());

					sleepTimer = 0;

				}

				else			//no room for sandworm on map

					sleep();	//try again later

			}

		}

	}

}



bool Sandworm::canAttack(ObjectClass* object)

{

	if (!object->wasDestroyed()

		&& object->isAGroundUnit()

		&& (object->getItemID() != Unit_Sandworm)	//wont kill other sandworms

		&& object->isVisible(getOwner()->getTeam())

		//&& (object->getOwner()->getTeam() != owner->getTeam())

		&& map->cellExists(object->getLocation())

		&& canPass(object->getLocation()->x, object->getLocation()->y)

		&& (map->getCell(object->getLocation())->getSandRegion() == map->getCell(&location)->getSandRegion()))

		return true;

	else

		return false;

}



bool Sandworm::canPass(int xPos, int yPos)

{

	return (map->cellExists(xPos, yPos)

			&& !map->getCell(xPos, yPos)->isRock()

			&& (!map->getCell(xPos, yPos)->hasAnUndergroundUnit()

				|| (map->getCell(xPos, yPos)->getUndergroundUnit() == this)));

}



ObjectClass* Sandworm::findTarget()

{

	double closestDistance = 1000000.0;

	ObjectClass	*closestTarget = NULL;



	if (attackMode == AGGRESSIVE)

	{

		if (!unitList->isEmpty())

		{

			UnitClass* tempUnit;



			unitList->saveCurrent();

			unitList->reset();

			while(unitList->currentNotNull())

			{

				tempUnit = (UnitClass*)unitList->getCurrent();

				if (canAttack(tempUnit)

					&& (blockDistance(&location, tempUnit->getLocation()) < closestDistance))

				{

					closestTarget = tempUnit;

					closestDistance = blockDistance(&location, tempUnit->getLocation());

				}

				unitList->nextLink();

			}

			unitList->restoreCurrent();

		}

	}

	else

	{

		closestTarget = ObjectClass::findTarget();

		if (closestTarget != NULL)

			attackMode = AGGRESSIVE;	//now have been awoken, go forth and kill

	}



	return closestTarget;

}

