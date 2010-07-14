#include "sand.h"

#include "Items.h"

extern SDL_Surface *fogSurf;

extern bool fog_wanted;

TerrainClass::TerrainClass()

{

	int i;



	difficulty = 1;

	hideTile = 0;

	owner = NONE;

	spice = 0;

	tile = 0;

	sandRegion = NONE;



	sprite = NULL;

	explored = new bool[MAX_PLAYERS];

	/*fogged = new bool[MAX_PLAYERS];*/

	lastAccess = new clock_t[MAX_PLAYERS];

	for (i = 0; i < MAX_PLAYERS; i++)

	{

		explored[i] = false;

		/*fogged[i] = false;*/

		lastAccess[i] = -1;
	}



	location.x = location.y = 0;

	visited = false;



	next = NULL;

	previous = NULL;

}

TerrainClass::~TerrainClass()

{

	delete[] explored;

/*	delete[] fogged;*/

}



void TerrainClass::assignAirUnit(ObjectClass* newObject)

{

	assignedAirUnits.insertLast(newObject);

	//assignedAirUnit = newObject;

}



void TerrainClass::assignDeadObject(ObjectClass* newObject)

{

	assignedDeadObjects.insertLast(newObject);

}



void TerrainClass::assignNonInfantryGroundObject(ObjectClass* newObject)

{

	assignedNonInfantryGroundObjects.insertLast(newObject);

}



int TerrainClass::assignInfantry(ObjectClass* newObject, int currentPosition)

{

	int i = currentPosition;



	if (hasInfantry())

	{

		bool used[NUM_INFANTRY_PER_CELL];

		int pos;

		for (i = 0; i < NUM_INFANTRY_PER_CELL; i++)

			used[i] = false;



		assignedInfantry.reset();

		while (assignedInfantry.currentNotNull())

		{

			pos = ((InfantryClass*)assignedInfantry.getCurrent())->getCellPosition();

			if ((pos >= 0) && (pos < NUM_INFANTRY_PER_CELL))

				used[pos] = true;

			assignedInfantry.nextLink();

		}



		if (!used[currentPosition])

			i = currentPosition;

		else

		{

			for (i = 0; i < NUM_INFANTRY_PER_CELL; i++)

				if (!used[i])

					break;



			if ((i < 0) || (i >= NUM_INFANTRY_PER_CELL))

				i = 0;

		}

	}



	assignedInfantry.insertLast(newObject);

	return i;

}



void TerrainClass::assignUndergroundUnit(ObjectClass* newObject)

{

	assignedUndergroundUnits.insertLast(newObject);

}



void TerrainClass::blitToScreen(int xPos, int yPos)

{

//	COORDTYPE	cellLocation = { xPos + dborder->minX/BLOCKSIZE, yPos + dborder->minY/BLOCKSIZE };

	SDL_Rect	source,

				drawLocation;



	source.y = 0;

	source.w = source.h = drawLocation.w = drawLocation.h = BLOCKSIZE;


	if (hasANonInfantryGroundObject() && getNonInfantryGroundObject()->isAStructure())

	{	//if got a structure, draw the structure, and dont draw any terrain because wont be seen

		bool	done = false;	//only draw it once

		int		i, j;

		StructureClass* structure = (StructureClass*)getNonInfantryGroundObject();

		for (i = structure->getX(); (i < structure->getX() + structure->getStructureSizeX()) && !done;  i++)

		for (j = structure->getY(); (j < structure->getY() + structure->getStructureSizeY()) && !done;  j++)

			if ((i >= dborder->minX/BLOCKSIZE) && (i <= dborder->maxX/BLOCKSIZE) && (j >= dborder->minY/BLOCKSIZE) && (j <= dborder->maxY/BLOCKSIZE)

				&& map->cellExists(i, j) && (map->getCell(i, j)->isExplored(thisPlayer->getPlayerNumber()) || debug))

			{

				structure->setFogged(isFogged(thisPlayer->getPlayerNumber()));
				if ((i == location.x) && (j == location.y))

					structure->blitToScreen();	//only this cell will draw it, so will be drawn only once
				
				done = true;

			}

	}

	else	//draw terrain

	{

		source.x = tile*BLOCKSIZE;

		drawLocation.x = xPos;

		drawLocation.y = yPos;

		
		SDL_BlitSurface(sprite, &source, screen, &drawLocation);
		
		if(!isFogged(thisPlayer->getPlayerNumber()))
		{
	
		if (damage[0].damageType != NONE)

		{

			int i = 0;

			while((damage[i].damageType != NONE) && (i < DAMAGEPERCELL))

			{

				source.x = damage[i].tile*BLOCKSIZE;

				drawLocation.x = damage[i].realPos.x - dborder->minX - BLOCKSIZE/2;

				drawLocation.y = damage[i].realPos.y - dborder->minY - BLOCKSIZE/2;

								
				SDL_BlitSurface((SDL_Surface*)dataFile[damage[i].damageType].dat, &source, screen, &drawLocation);
				
				i++;

			}
			
		}
		
			
		

	
		if (hasAnUndergroundUnit() && getUndergroundUnit()->isVisible(thisPlayer->getTeam()))

			undergroundUnitBlitList->insertLast(getUndergroundUnit());



		if (hasADeadObject())

		{

			ObjectClass* current;

			assignedDeadObjects.reset();

			while (assignedDeadObjects.currentNotNull())

			{

				current = (ObjectClass*)assignedDeadObjects.getCurrent();

				if (current->isVisible(thisPlayer->getTeam()))

					infantryBlitList->insertLast(current);

				assignedDeadObjects.nextLink();

			}

		}



		if (hasInfantry())

		{

			InfantryClass* current;

			assignedInfantry.reset();

			while (assignedInfantry.currentNotNull())

			{

				current = (InfantryClass*)assignedInfantry.getCurrent();

				if (current->isVisible(thisPlayer->getTeam()))

					infantryBlitList->insertLast(current);

				assignedInfantry.nextLink();

			}

		}



		if (hasANonInfantryGroundObject())

		{

			ObjectClass* current;



			assignedNonInfantryGroundObjects.reset();

			while (assignedNonInfantryGroundObjects.currentNotNull())

			{

				current = (ObjectClass*)assignedNonInfantryGroundObjects.getCurrent();

				if (current->isVisible(thisPlayer->getTeam()))

					groundUnitBlitList->insertLast(current);

				assignedNonInfantryGroundObjects.nextLink();

			}

		}

	}



	if (!assignedAirUnits.isEmpty())

	{

		AirUnit*	airUnit;



		assignedAirUnits.reset();

		while (assignedAirUnits.currentNotNull())

		{

			airUnit = (AirUnit*)assignedAirUnits.getCurrent();

			if (airUnit->isVisible(thisPlayer->getTeam()))

				airUnitBlitList->insertLast(airUnit);

			assignedAirUnits.nextLink();

		}
	
	}
	/*draw fog*/
	if(isFogged(thisPlayer->getPlayerNumber()))
	SDL_BlitSurface(fogSurf, &source, screen, &drawLocation);	
}	

	
}




void TerrainClass::clearDamage()

{

	damagePos = 0;

	for(int i=0; i<DAMAGEPERCELL; i++)

		damage[i].damageType = NONE;

}



void TerrainClass::damageCell(ObjectClass* damager, PlayerClass* damagerOwner, COORDTYPE* realPos, int bulletType, int bulletDamage, int damagePiercing, int damageRadius, bool air)

{

	TerrainClass* cell;



	if (bulletType == Unit_Sandworm)

	{

		ObjectClass* object;

		if (hasADeadObject())

		{

			assignedDeadObjects.reset();

			while (assignedDeadObjects.currentNotNull())

			{

				object = (ObjectClass*)assignedDeadObjects.getCurrent();

				if ((object->getX() == location.x) && (object->getY() == location.y))

					object->setVisible(VIS_ALL, false);

				assignedDeadObjects.nextLink();

			}

		}



		if (hasInfantry())

		{

			assignedInfantry.reset();

			while (assignedInfantry.currentNotNull())

			{

				object = (ObjectClass*)assignedInfantry.getCurrent();

				if ((object->getX() == location.x) && (object->getY() == location.y))

				{

					object->setVisible(VIS_ALL, false);

					object->handleDamage(bulletDamage, damager);

				}

				assignedInfantry.nextLink();

			}



		}



		if (hasANonInfantryGroundObject())

		{

			assignedNonInfantryGroundObjects.reset();

			while (assignedNonInfantryGroundObjects.currentNotNull())

			{

				object = (ObjectClass*)assignedNonInfantryGroundObjects.getCurrent();

				if ((object->getX() == location.x) && (object->getY() == location.y))

				{

					object->setVisible(VIS_ALL, false);

					object->handleDamage(bulletDamage, damager);

				}

				assignedNonInfantryGroundObjects.nextLink();

			}



		}

	}

	else

	{

		int			distance;

		double		damageProp;

		COORDTYPE	centrePoint;



		if (air)

		{

			if (hasAnAirUnit() && ((bulletType == Bullet_DRocket) || (bulletType == Bullet_Rocket) || (bulletType == Bullet_SmallRocket)))

			{

				AirUnit*	airUnit;



				assignedAirUnits.reset();

				while(assignedAirUnits.currentNotNull())

				{

					airUnit = (AirUnit*)assignedAirUnits.getCurrent();

					airUnit->getCentrePoint(&centrePoint);

					distance = round(distance_from(&centrePoint, realPos));

					if (distance <= 0)

						distance = 1;

					if ((distance - airUnit->getRadius()) <= damageRadius)

					{

						if ((bulletType == Bullet_DRocket) && (getRandomInt(0, 100) <= 30))

							((UnitClass*)airUnit)->netDeviate(damagerOwner);



						damageProp = ((double)(damageRadius + airUnit->getRadius() - distance))/((double)distance);

						if (damageProp > 0)

						{

							if (damageProp > 1.0)

								damageProp = 1.0;

							airUnit->handleDamage(round((double)(bulletDamage + damagePiercing) * damageProp) - airUnit->getArmour(), damager);

						}

					}



					assignedAirUnits.nextLink();

				}

			}

		}

		else

		{

			if (hasANonInfantryGroundObject())

			{

				ObjectClass* object;



				assignedNonInfantryGroundObjects.reset();

				while (assignedNonInfantryGroundObjects.currentNotNull())

				{

					object = (ObjectClass*)assignedNonInfantryGroundObjects.getCurrent();

					object->getClosestCentrePoint(&centrePoint, &location);

					distance = round(distance_from(&centrePoint, realPos));

					if (distance <= 0)

						distance = 1;

					if (distance - object->getRadius() <= damageRadius)

					{

						if ((bulletType == Bullet_DRocket) && (object->isAUnit()) && (getRandomInt(0, 100) <= 30))

							((UnitClass*)object)->netDeviate(damagerOwner);



						damageProp = ((double)(damageRadius + object->getRadius() - distance))/((double)distance);

						if (damageProp > 0)

						{

							if (damageProp > 1.0)

								damageProp = 1.0;

							object->handleDamage(round((double)(bulletDamage + damagePiercing) * damageProp) - object->getArmour(), damager);

						}

					}

					assignedNonInfantryGroundObjects.nextLink();

				}

			}



			if (hasInfantry())

			{

				InfantryClass* infantry;



				assignedInfantry.reset();

				while (assignedInfantry.currentNotNull())

				{

					infantry = (InfantryClass*)assignedInfantry.getCurrent();

					infantry->getCentrePoint(&centrePoint);

					distance = round(distance_from(&centrePoint, realPos));

					if (distance <= 0)

						distance = 1;

					if (distance - infantry->getRadius() <= damageRadius)

					{

						if ((bulletType == Bullet_DRocket) && (getRandomInt(0, 100) <= 30))

							((UnitClass*)infantry)->netDeviate(damagerOwner);



						damageProp = ((double)(damageRadius + infantry->getRadius() - distance))/((double)distance);

						if (damageProp > 0)

						{

							if (damageProp > 1.0)

								damageProp = 1.0;

							infantry->handleDamage(round((double)(bulletDamage + damagePiercing) * damageProp) - infantry->getArmour(), damager);

						}

					}

					assignedInfantry.nextLink();

				}

			}



			if (hasAnUndergroundUnit())

			{

				ObjectClass* object;



				assignedUndergroundUnits.reset();

				while (assignedUndergroundUnits.currentNotNull())

				{

					object = (ObjectClass*)assignedUndergroundUnits.getCurrent();

					object->getCentrePoint(&centrePoint);

					distance = round(distance_from(&centrePoint, realPos));

					if (distance <= 0)

						distance = 1;

					if (distance - object->getRadius() <= damageRadius)

					{

						if ((bulletType == Bullet_DRocket) && (getRandomInt(0, 100) <= 30))

							((UnitClass*)object)->netDeviate(damagerOwner);

						

						damageProp = ((double)(damageRadius + object->getRadius() - distance))/((double)distance);

						if (damageProp > 0)

						{

							if (damageProp > 1.0)

								damageProp = 1.0;

							object->handleDamage(round((double)(bulletDamage + damagePiercing) * damageProp) - object->getArmour(), damager);

						}

					}

					assignedUndergroundUnits.nextLink();

				}

			}

		}



		if (!air)

		{

			if ((getType() == Terrain_Sand)

				&& ((getTile() == Terrain_a2) || (getTile() == Terrain_a3)))

			{	//a spice bloom

				int i, j;



				setType(Terrain_Spice);

				soundPlayer->playSoundAt(Sound_bloom, getLocation());



				for (i = -6; i <= 6; i++) 

				for (j = -6; j <= 6; j++)

				if (map->cellExists(location.x + i, location.y + j)

					&& (distance_from(location.x, location.y, location.x + i, location.y + j) <= 6))

				{

					cell = map->getCell(location.x + i, location.y + j);

					if (cell->isSand())

						cell->setType(Terrain_Spice);

				}



				for(i = location.x-8; i <= location.x+8; i++)

				for(j = location.y-8; j <= location.y+8; j++)

				if (map->cellExists(i, j))

					smooth_spot(i, j);

			}



			if (map->cellExists(realPos->x/BLOCKSIZE, realPos->y/BLOCKSIZE))

			{

				cell = map->getCell(realPos->x/BLOCKSIZE, realPos->y/BLOCKSIZE);

			

				if (((bulletType == Bullet_Rocket) || (bulletType == Bullet_SmallRocket) || (bulletType == Bullet_LargeRocket) || (bulletType == Unit_Devastator))

					&& (!hasAGroundObject() || !getGroundObject()->isAStructure())

					&& ((realPos->x <= (location.x*BLOCKSIZE + BLOCKSIZE/2))//if hasn't been assigned an object or the assigned object isnt a structure

						&& (realPos->y <= (location.y*BLOCKSIZE + BLOCKSIZE/2))))

				{

					if (!cell->hasAGroundObject() || !cell->getGroundObject()->isAStructure())

					{

						if (((cell->getType() == Terrain_Rock) && (cell->getTile() == Terrain_t1))

							|| (cell->getType() == Structure_Slab1))

						{

							if (cell->getType() == Structure_Slab1)

							{

								cell->setType(Terrain_Rock);

								cell->setOwner(NONE);

							}



							if (bulletType == Bullet_SmallRocket)

								damage[damagePos%DAMAGEPERCELL].tile = Terrain_td1;

							else

								damage[damagePos%DAMAGEPERCELL].tile = Terrain_td2;

							damage[damagePos%DAMAGEPERCELL].damageType = Terrain_RockDamage;

							damage[damagePos%DAMAGEPERCELL].realPos.x = realPos->x;

							damage[damagePos%DAMAGEPERCELL].realPos.y = realPos->y;

							damagePos++;

						}

						else if ((cell->getType() == Terrain_Sand) || (cell->getType() == Terrain_Spice))

						{

							if (bulletType == Bullet_SmallRocket)

								damage[damagePos%DAMAGEPERCELL].tile = getRandomInt(Terrain_sd1, Terrain_sd2);

							else

								damage[damagePos%DAMAGEPERCELL].tile = Terrain_sd3;

							damage[damagePos%DAMAGEPERCELL].damageType = Terrain_SandDamage;

							damage[damagePos%DAMAGEPERCELL].realPos.x = realPos->x;

							damage[damagePos%DAMAGEPERCELL].realPos.y = realPos->y;

							damagePos++;

						}

					}

				}

			}

		}

	}

}



void TerrainClass::selectAllPlayersUnits(int playerNum, ObjectClass** lastCheckedObject, ObjectClass** lastSelectedObject)

{

	if (hasInfantry())

	{

		assignedInfantry.reset();

		while (assignedInfantry.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedInfantry.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected())

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedInfantry.nextLink();

		}

	}



	if (hasANonInfantryGroundObject())

	{

		assignedNonInfantryGroundObjects.reset();

		while(assignedNonInfantryGroundObjects.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedNonInfantryGroundObjects.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected() && (*lastCheckedObject)->isAUnit())

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedNonInfantryGroundObjects.nextLink();

		}

	}



	if (hasAnUndergroundUnit())

	{

		assignedUndergroundUnits.reset();

		while(assignedUndergroundUnits.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedUndergroundUnits.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected())

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedUndergroundUnits.nextLink();

		}

	}

		

	if (hasAnAirUnit())

	{

		assignedAirUnits.reset();

		while(assignedAirUnits.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedAirUnits.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected())

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedAirUnits.nextLink();

		}

	}

}



void TerrainClass::selectAllPlayersUnitsOfType(int playerNum, int itemID, ObjectClass** lastCheckedObject, ObjectClass** lastSelectedObject)

{

	if (hasInfantry())

	{

		assignedInfantry.reset();

		while (assignedInfantry.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedInfantry.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected() && ((*lastCheckedObject)->getItemID() == itemID))

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedInfantry.nextLink();

		}

	}



	if (hasANonInfantryGroundObject())

	{

		assignedNonInfantryGroundObjects.reset();

		while(assignedNonInfantryGroundObjects.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedNonInfantryGroundObjects.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected() && ((*lastCheckedObject)->getItemID() == itemID))

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedNonInfantryGroundObjects.nextLink();

		}

	}



	if (hasAnUndergroundUnit())

	{

		assignedUndergroundUnits.reset();

		while(assignedUndergroundUnits.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedUndergroundUnits.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected() && ((*lastCheckedObject)->getItemID() == itemID))

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedUndergroundUnits.nextLink();

		}

	}

		

	if (hasAnAirUnit())

	{

		assignedAirUnits.reset();

		while(assignedAirUnits.currentNotNull())

		{

			*lastCheckedObject = (ObjectClass*)assignedAirUnits.getCurrent();

			if (((*lastCheckedObject)->getOwner()->getPlayerNumber() == playerNum) && !(*lastCheckedObject)->isSelected() && ((*lastCheckedObject)->getItemID() == itemID))

			{

				(*lastCheckedObject)->setSelected(true);

				selectedList->insertLast(*lastCheckedObject);

				(*lastSelectedObject) = *lastCheckedObject;

			}

			assignedAirUnits.nextLink();

		}

	}

}



void TerrainClass::unassignDeadObject(ObjectClass* newObject)

{

	if (hasADeadObject())

	{

		assignedDeadObjects.reset();

		while (assignedDeadObjects.currentNotNull())

		{

			if (assignedDeadObjects.getCurrent() == newObject)

			{

				assignedDeadObjects.removeCurrent();

				return;

			}

			assignedDeadObjects.nextLink();

		}

	}

}



void TerrainClass::unassignNonInfantryGroundObject(ObjectClass* newObject)

{

	if (hasANonInfantryGroundObject())

	{

		assignedNonInfantryGroundObjects.reset();

		while(assignedNonInfantryGroundObjects.currentNotNull())

		{

			if (newObject == assignedNonInfantryGroundObjects.getCurrent())

			{

				assignedNonInfantryGroundObjects.removeCurrent();

				return;

			}

			assignedNonInfantryGroundObjects.nextLink();

		}

	}

}



void TerrainClass::unassignObject(ObjectClass* newObject)

{

	if (newObject->isInfantry())

	{

		if (hasInfantry())

		{

			assignedInfantry.reset();

			while (assignedInfantry.currentNotNull())

			{

				if (assignedInfantry.getCurrent() == newObject)

				{

					assignedInfantry.removeCurrent();

					return;

				}

				assignedInfantry.nextLink();

			}

		}

	}

	else if (newObject->getItemID() == Unit_Sandworm)

	{

		if (hasAnUndergroundUnit())

		{

			assignedUndergroundUnits.reset();

			while(assignedUndergroundUnits.currentNotNull())

			{

				if (newObject == assignedUndergroundUnits.getCurrent())

				{

					assignedUndergroundUnits.removeCurrent();

					return;

				}

				assignedUndergroundUnits.nextLink();

			}

		}

		

	}

	else if (newObject->isAGroundUnit() || newObject->isAStructure())

	{

		if (hasANonInfantryGroundObject())

		{

			assignedNonInfantryGroundObjects.reset();

			while(assignedNonInfantryGroundObjects.currentNotNull())

			{

				if (newObject == assignedNonInfantryGroundObjects.getCurrent())

				{

					assignedNonInfantryGroundObjects.removeCurrent();

					return;

				}

				assignedNonInfantryGroundObjects.nextLink();

			}

		}

	}

	else if (newObject->isAFlyingUnit())

	{

		if (hasAnAirUnit())

		{

			assignedAirUnits.reset();

			while(assignedAirUnits.currentNotNull())

			{

				if (newObject == assignedAirUnits.getCurrent())

				{

					assignedAirUnits.removeCurrent();

					return;

				}

				assignedAirUnits.nextLink();

			}

		}

	}

}



void TerrainClass::setType(int newType)

{

	type = newType;

	tile = 0;



	sprite = (SDL_Surface*)(dataFile[type].dat);



	if ((type == Terrain_Spice) || (type == Terrain_ThickSpice) )

	{

		damageType = Terrain_SandDamage;

		difficulty = 1.2;

		spice = getRandomInt(RANDOMSPICEMIN, RANDOMSPICEMAX);

	}

	else if (type == Terrain_Dunes)

	{

		damageType = Terrain_SandDamage;

		difficulty = 1.5;

	}

	else

	{

		difficulty = 1.2;

		spice = 0;

		

		if (isRock())

		{

			difficulty = 1.0;

			sandRegion = NONE;

			damageType = Terrain_RockDamage;



			if (hasAnUndergroundUnit())

			{

				assignedUndergroundUnits.reset();

				while(assignedUndergroundUnits.currentNotNull())

					((Sandworm*)assignedUndergroundUnits.removeCurrent())->destroy();

			}



			if (type == Structure_Slab1)

			{

				difficulty = 0.7;

				tile = 0;	//theres only the one image - the concrete block (BLOCKSIZE*BLOCKSIZE)

			}

			else if (type == Terrain_Mountain)

			{

				difficulty = 1.5;



				if (hasANonInfantryGroundObject())

				{

					assignedNonInfantryGroundObjects.reset();

					while(assignedNonInfantryGroundObjects.currentNotNull())

						((GroundUnit*)assignedNonInfantryGroundObjects.removeCurrent())->destroy();

				}

			}

		}

	
	}



	/*

	clearDamage();

	for (int i=location.x-1; i <= location.x+1; i++)

	for (int j=location.y-1; j <= location.y+1; j++)

		if (map->cellExists(i, j))

		{

			showMessage("clearing damage-terrainclass.cpp");

			map->getCell(i, j)->clearDamage();

		}*/



	/*Hack, clears 3x3 no matter what you place...*/

	//char temp[50];

	clearDamage();

	

	for (int i=location.x; i <= location.x+3; i++)

	{

		for (int j=location.y; j <= location.y+3; j++)

		{

			if (map->cellExists(i, j))

			{

				//sprintf(temp, "clear x: %d, y: %d", i, j);

				//showMessage(temp);

				map->getCell(i, j)->clearDamage();

			}

		}

	}

}



void TerrainClass::squash()

{

	if (hasInfantry() && (settings.playerType != CLIENT))

	{

		assignedInfantry.reset();

		while (assignedInfantry.currentNotNull())

			((InfantryClass*)assignedInfantry.getCurrent())->squash();

	}

}



bool TerrainClass::hasAnObject()

{

	return (hasAGroundObject() || hasAnAirUnit() || hasInfantry() || hasAnUndergroundUnit());

}



int TerrainClass::getInfantryTeam()

{

	int team = NONE;

	if (hasInfantry())

		team = getInfantry()->getOwner()->getTeam();



	return team;

}



double TerrainClass::harvestSpice()

{

	double oldSpice = spice;

	

	if ((spice - HARVESTSPEED*5) >= 0)

		spice -= HARVESTSPEED*5;

	else

		spice = 0;

	

	return (oldSpice - spice);

}



ObjectClass* TerrainClass::getAirUnit()

{

	return (ObjectClass*)assignedAirUnits.getFirstElement();

}



ObjectClass* TerrainClass::getDeadObject()

{

	return (ObjectClass*)assignedDeadObjects.getFirstElement();

}



ObjectClass* TerrainClass::getGroundObject()

{

	if (hasANonInfantryGroundObject())

		return getNonInfantryGroundObject();

	else if (hasInfantry())

		return getInfantry();

	else

		return NULL;

}



ObjectClass* TerrainClass::getInfantry()

{

	return (ObjectClass*)assignedInfantry.getFirstElement();

}



ObjectClass* TerrainClass::getNonInfantryGroundObject()

{

	return (ObjectClass*)assignedNonInfantryGroundObjects.getFirstElement();

}



ObjectClass* TerrainClass::getUndergroundUnit()

{

	return (ObjectClass*)assignedUndergroundUnits.getFirstElement();

}



/*ObjectClass* TerrainClass::getInfantry(int i)

{

	int count;

	InfantryClass* infantry;



	assignedInfantry.reset();

	while (assignedInfantry.currentNotNull())

	{

		((InfantryClass*)assignedInfantry.getCurrent())->squash();

		assignedInfantry.nextLink();

	}

	return assignedInfantry.removeElement();

}*/



ObjectClass* TerrainClass::getObject()

{

	ObjectClass* temp = NULL;

	if (hasAnAirUnit())

		temp = getAirUnit();

	else if (hasANonInfantryGroundObject())

		temp = getNonInfantryGroundObject();

	else if (hasInfantry())

		temp = getInfantry();

	else if (hasAnUndergroundUnit())

		temp = getUndergroundUnit();



	return temp;

}



ObjectClass* TerrainClass::getObjectAt(int x, int y)

{

	ObjectClass* temp = NULL;

	if (hasAnAirUnit())

		temp = getAirUnit();

	else if (hasANonInfantryGroundObject())

		temp = getNonInfantryGroundObject();

	else if (hasInfantry())

	{

		double closestDistance = NONE;

		COORDTYPE atPos, centrePoint;

		InfantryClass* infantry;

		atPos.x = x;

		atPos.y = y;

		

		assignedInfantry.reset();

		while (assignedInfantry.currentNotNull())

		{

			infantry = (InfantryClass*)assignedInfantry.getCurrent();

			infantry->getCentrePoint(&centrePoint);

			if ((closestDistance == NONE) || (distance_from(&atPos, &centrePoint) < closestDistance))

			{

				closestDistance = distance_from(&atPos, &centrePoint);

				temp = infantry;

			}

			assignedInfantry.nextLink();

		}

	}

	else if (hasAnUndergroundUnit())

		temp = getUndergroundUnit();



	return temp;

}



ObjectClass* TerrainClass::getObjectWithID(int objectID)

{

	ObjectClass* object;



	if (hasInfantry())

	{

		assignedInfantry.reset();

		while (assignedInfantry.currentNotNull())

		{

			object = (ObjectClass*)assignedInfantry.getCurrent();

			if (object->hasObjectID(objectID))

				return object;

			assignedInfantry.nextLink();

		}

	}



	if (hasANonInfantryGroundObject())

	{

		assignedNonInfantryGroundObjects.reset();

		while(assignedNonInfantryGroundObjects.currentNotNull())

		{

			object = (ObjectClass*)assignedNonInfantryGroundObjects.getCurrent();

			if (object->hasObjectID(objectID))

				return object;

			assignedNonInfantryGroundObjects.nextLink();

		}

	}



	if (hasAnUndergroundUnit())

	{

		assignedUndergroundUnits.reset();

		while(assignedUndergroundUnits.currentNotNull())

		{

			object = (ObjectClass*)assignedUndergroundUnits.getCurrent();

			if (object->hasObjectID(objectID))

				return object;

			assignedUndergroundUnits.nextLink();

		}

	}



	if (hasAnAirUnit())

	{

		assignedAirUnits.reset();

		while(assignedAirUnits.currentNotNull())

		{

			object = (ObjectClass*)assignedAirUnits.getCurrent();

			if (object->hasObjectID(objectID))

				return object;

			assignedAirUnits.nextLink();

		}

	}

		

	if (hasADeadObject())

	{

		assignedDeadObjects.reset();

		while(assignedDeadObjects.currentNotNull())

		{

			object = (ObjectClass*)assignedDeadObjects.getCurrent();

			if (object->hasObjectID(objectID))

				return object;

			assignedDeadObjects.nextLink();

		}

	}

	return NULL;

}



/*

ObjectClass* TerrainClass::selectAll()

{

}*/

bool TerrainClass::isFogged(int player)
{
#ifdef	_DEBUG
		if(debug) 
		return false;
#endif

	if(!fog_wanted)
		return false;
	else

	if((int)((clock() - lastAccess[player])/CLOCKS_PER_SEC) >= FOGTIMEOUT)
		return true;
	else 
	return false; 
	
	
}

Uint32
TerrainClass::getRadarColour(int player, bool radar)
{
	int colour;
	
	if (isExplored(player) ||debug || (!shade) )
			
	if(isFogged(player) && radar)
		return fogColour;
	else	
	{
		
			{
				if (hasAnObject())
				{
					ObjectClass* tempObject = getObject();
		
					switch (tempObject->getOwner()->getColour())
					{
						case(HOUSE_ATREIDES): colour = COLOUR_ATREIDES; break;
						case(HOUSE_ORDOS): colour = COLOUR_ORDOS; break;
						case(HOUSE_HARKONNEN): colour = COLOUR_HARKONNEN; break;
						case(HOUSE_SARDAUKAR): colour = COLOUR_SARDAUKAR; break;
						case(HOUSE_FREMEN): colour = COLOUR_FREMEN; break;
						case(HOUSE_MERCENARY): colour = COLOUR_MERCENARY;
					}
			
					if (tempObject->getItemID() == Unit_Sandworm)
						colour = COLOUR_WHITE;
						
				/*units are not visible if no radar*/
				if(!radar && (tempObject->getOwner()->getPlayerNumber() != player))
					colour = COLOUR_BLACK;
				
				}
				else
				{
				if(!radar && !debug) 
					return COLOUR_BLACK;
				
				switch (getType())
					{
						case Terrain_Dunes:
							colour = COLOUR_DESERTSAND;
							break;
						case Terrain_Mountain:
							colour = COLOUR_MOUNTAIN;
							break;
						case Terrain_Rock:
							colour = COLOUR_DARKGREY;
						break;
						case Terrain_Sand:
							colour = COLOUR_DESERTSAND;
							break;
						case Terrain_Spice:
							colour = COLOUR_SPICE;
							break;
						case Terrain_ThickSpice:
							colour = COLOUR_THICKSPICE;
							break;
						default://case Structure_Slab1:
							colour = COLOUR_DARKGREY;
				
				};
			};
		}
		
	
	fogColour = colour;
	
	}						
	else 
	colour = COLOUR_BLACK;						

return colour;
};

