#include "sand.h"
#include "Items.h"

AiPlayerClass::AiPlayerClass(int newPlayerNumber, int newHouse, int newColour, int newCredits, DIFFICULTYTYPE difficulty) : PlayerClass(newPlayerNumber, newHouse, newColour, newCredits)
{
	ai = true;

	attackTimer = getRandomInt(20000, 40000);
	buildTimer = 0;

	setDifficulty(difficulty);
}
AiPlayerClass::~AiPlayerClass()
{
}

void AiPlayerClass::addCredits(double newCredits)
{
	if (newCredits > 0.0)
	{
		double	oldCredits = credits;

		credits += newCredits*spiceMultiplyer;
		if (this == thisPlayer)
		{
			if (settings.winFlags != 3)
			{
				if (credits >= 1000)
				{
					if (settings.winFlags == 6)
						win();
					else if (credits >= 2700)
						win();
				}
			}
		}
	}
}

void AiPlayerClass::clearPlaceList(int num)
{
	COORDTYPE* temp = (COORDTYPE*)placeLocations.removeElement(placeLocations.getElement(num));

	if (temp)
		delete temp;
}

void AiPlayerClass::decrementStructures(int itemID, COORDTYPE* location)
{
	PlayerClass::decrementStructures(itemID, location);

	//rebuild the structure if its the original gameType
	if (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && !structureList->isEmpty())
	{
		bool done = false;
		StructureClass	*structure;

		structureList->saveCurrent();
		structureList->reset();
		while (!done && structureList->currentNotNull())
		{
			structure = (StructureClass*)structureList->getCurrent();
			if ((structure->getItemID() == Structure_ConstructionYard) && (structure->getOwner() == this))
			{
				COORDTYPE *newLocation;

				if (settings.concreteRequired)
				{
					int i, j,
						incI, incJ,
						startI, startJ;
					BuildItemClass* item = new BuildItemClass(itemID);

					if (map->isWithinBuildRange(location->x, location->y, this))
						startI = location->x, startJ = location->y, incI = 1, incJ = 1;
					else if (map->isWithinBuildRange(location->x + item->getStructureSizeX() - 1, location->y, this))
						startI = location->x + item->getStructureSizeX() - 1, startJ = location->y, incI = -1, incJ = 1;
					else if (map->isWithinBuildRange(location->x, location->y + item->getStructureSizeY() - 1, this))
						startI = location->x, startJ = location->y + item->getStructureSizeY() - 1, incI = 1, incJ = -1;
					else
						startI = location->x + item->getStructureSizeX() - 1, startJ = location->y + item->getStructureSizeY() - 1, incI = -1, incJ = -1;

					for (i = startI; abs(i - startI) < item->getStructureSizeX(); i += incI)
					for (j = startJ; abs(j - startJ) < item->getStructureSizeY(); j += incJ)
						if (map->getCell(i, j)->getType() != Structure_Slab1)
						{
							newLocation = new COORDTYPE;
							newLocation->x = i;
							newLocation->y = j;
							placeLocations.insertLast(newLocation);
							((ConstructionYardClass*)structure)->buildType(Structure_Slab1);
						}

					delete item;
				}

				((ConstructionYardClass*)structure)->buildType(itemID);
				newLocation = new COORDTYPE;
				newLocation->x = location->x;
				newLocation->y = location->y;
				placeLocations.insertLast(newLocation);
				done = true;
			}
			structureList->nextLink();
		}
		structureList->restoreCurrent();
	}
}

void AiPlayerClass::load(FILE* fp)
{
	int		i, j;
	COORDTYPE*	tempCoord;

	fread(&j, sizeof(int), 1, fp);
	setAttackTimer(j);
	fread(&j, sizeof(int), 1, fp);
	for(i = 0; i < j; i++)
	{
		tempCoord = new COORDTYPE;
		fread(tempCoord, sizeof(COORDTYPE), 1, fp);
		placeLocations.insertLast(tempCoord);
	}

	PlayerClass::load(fp);
}

void AiPlayerClass::noteDamageLocation(void* object, COORDTYPE* location)
{
	int itemID = ((ObjectClass*)object)->getItemID();

	if (settings.playerType != CLIENT)
	{
		if ((itemID == Unit_Harvester) 
			|| ((itemID >= Structure_Barracks)
				&& (itemID <= Structure_WOR)))
		{	//scramble some free units to defend
			if (!unitList->isEmpty())
			{
				UnitClass* tempUnit;

				unitList->saveCurrent();
				unitList->reset();
				while(unitList->currentNotNull())
				{
					tempUnit = (UnitClass*)unitList->getCurrent();
					if (tempUnit->isRespondable()
						&& (tempUnit->getOwner() == this)
						&& !tempUnit->isAttacking()
						&& (tempUnit->getItemID() != Unit_Harvester)
						&& (tempUnit->getItemID() != Unit_MCV)
						&& (tempUnit->getItemID() != Unit_Carryall)
						&& (tempUnit->getItemID() != Unit_Saboteur)
						&& (tempUnit->getItemID() != Unit_Sandworm))
					{
						tempUnit->netDoCommand(NULL, location, false);
						//tempUnit->setAttacking(true);
					}
					unitList->nextLink();
				}
				unitList->restoreCurrent();
			}
		}
		else if (((ObjectClass*)object)->isAUnit() && ((UnitClass*)object)->isAttacking() && !unitList->isEmpty())
		{	//if one of attack force is shot, unrestrict the speeds of others as to catch up
			UnitClass	*damagedUnit = (UnitClass*)object,
						*tempUnit;

			unitList->saveCurrent();
			unitList->reset();
			while(unitList->currentNotNull())
			{
				tempUnit = (UnitClass*)unitList->getCurrent();
				if (tempUnit->isRespondable()
					&& (tempUnit->getOwner() == this)
					&& tempUnit->isAttacking()
					&& (tempUnit->getItemID() != Unit_Harvester)
					&& (tempUnit->getItemID() != Unit_MCV)
					&& (tempUnit->getItemID() != Unit_Carryall)
					&& (tempUnit->getItemID() != Unit_Saboteur))
				{
					tempUnit->setSpeedCap(NONE);
				}
				unitList->nextLink();
			}
			unitList->restoreCurrent();
		}
	}
}

void AiPlayerClass::save(FILE* fp)
{
	int		j;
	double	tempDouble;
	COORDTYPE*	tempCoord;

	j = getHouse();
	fwrite(&j, sizeof(int), 1, fp);
	j = getColour();
	fwrite(&j, sizeof(int), 1, fp);

	j = 1;	//is an ai player
	fwrite(&j, sizeof(int), 1, fp);
	j = getDifficulty();
	fwrite(&j, sizeof(int), 1, fp);
	j = getAttackTimer();
	fwrite(&j, sizeof(int), 1, fp);
	j = placeLocations.getNumElements();
	fwrite(&j, sizeof(int), 1, fp);
	placeLocations.reset();
	while (placeLocations.currentNotNull())
	{
		tempCoord = (COORDTYPE*)placeLocations.getCurrent();
		fwrite(tempCoord, sizeof(COORDTYPE), 1, fp);

		placeLocations.nextLink();
	}

	j = getTeam();
	fwrite(&j, sizeof(int), 1, fp);
	if (this == thisPlayer)
		j = 1;
	else
		j = 0;
	fwrite(&j, sizeof(int), 1, fp);
	tempDouble = getStartingCredits();
	fwrite(&tempDouble, sizeof(double), 1, fp);
	tempDouble = getStoredCredits();
	fwrite(&tempDouble, sizeof(double), 1, fp);
}

void AiPlayerClass::setAttackTimer(int newAttackTimer)
{
	if (newAttackTimer >= 0)
		attackTimer = newAttackTimer;
}

void AiPlayerClass::setDifficulty(DIFFICULTYTYPE newDifficulty)
{
	if ((newDifficulty >= EASY) && (newDifficulty <= HARD))
	{
		difficulty = newDifficulty;

		switch (difficulty)
		{
		case EASY:
			spiceMultiplyer = 0.5;
			break;
		case MEDIUM:
			spiceMultiplyer = 0.75;
			break;
		case HARD:
			spiceMultiplyer = 1.0;
			break;
	/*	case IMPOSSIBLE:
			spiceMultiplyer = 1.25;
			break;*/
		default:
			break;
		}
	}
}

void AiPlayerClass::update()
{
	if (buildTimer == 0)
	{
		/*char temp[40];
		sprintf(temp, "Computer %d - Credits:%f", getPlayerNumber(), credits + startingCredits);
		showMessage(temp);*/
		if (!structureList->isEmpty())
		{
			BuilderClass* builder;
			StructureClass* tempStructure;
			structureList->reset();
			while(structureList->currentNotNull())
			{
				tempStructure = static_cast<StructureClass*>(structureList->getCurrent());

				//if this players structure, and its a heavy factory, build something
				if (tempStructure->getOwner() == this)
				{
					builder = (BuilderClass*)tempStructure;
					switch (tempStructure->getItemID())
					{
					case (Structure_Barracks): case (Structure_LightFactory): case (Structure_WOR):
						if (((credits + startingCredits) > 100) && (builder->getNumItemsToBuild() < 1) && (builder->getNumSelections() > 0))
							builder->buildRandom();
						break;
					case (Structure_ConstructionYard):
						//showMessage("doing check");
						if (((credits + startingCredits) > 100) && ((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)))
						{
							if ((builder->getNumItemsToBuild() < 1) && (builder->getNumSelections() > 0))
							{
								int itemID = NONE;
								BuildItemClass* item;
								if (power < 50)
									itemID = Structure_WindTrap;
								else if (numRefineries < 2)
									itemID = Structure_Refinery;
								else if (!hasRadar())
									itemID = Structure_Radar;
								else if (numWORs <= 0)
									itemID = Structure_WOR;
								else if (numRocketTurrets < 2)
									itemID = Structure_RocketTurret;
								else if (power < 150)
									itemID = Structure_WindTrap;
								else if (!hasLightFactory())
									itemID = Structure_LightFactory;
								else if (numRefineries < 3)
									itemID = Structure_Refinery;
								else if (numRocketTurrets < 4)
									itemID = Structure_RocketTurret;
								else if (numHeavyFactories <= 0)
									itemID = Structure_HeavyFactory;
								else if (numHighTechFactories <= 0)
									itemID = Structure_HighTechFactory;
								else if (numRocketTurrets < 6)
									itemID = Structure_RocketTurret;
								else if (numRepairYards <= 0)
									itemID = Structure_RepairYard;
								else if (numStarPorts <= 0)
									itemID = Structure_StarPort;
								else if (numIXs <= 0)
									itemID = Structure_IX;
								else if (numPalaces <= 0)
									itemID = Structure_Palace;
								else if (numRocketTurrets < 10)
									itemID = Structure_RocketTurret;
								else if (numWORs < 2)
									itemID = Structure_WOR;
								else if (numLightFactories < 2)
									itemID = Structure_LightFactory;
								else if (numHeavyFactories < 2)
									itemID = Structure_HeavyFactory;
								else if (numPalaces < 2)
									itemID = Structure_Palace;
								else if (power < 300)
									itemID = Structure_WindTrap;
								else if (numRocketTurrets < 20)
									itemID = Structure_RocketTurret;

								if (itemID != NONE)
								{
									
									COORDTYPE	placeLocation,
												*location;
									item = new BuildItemClass(itemID);

									location = findPlaceLocation(item->getStructureSizeX(), item->getStructureSizeY());
									if (location->x != NONE)
									{
										placeLocation = *location;
										if (settings.concreteRequired)
										{
											int i, j,
												incI, incJ,
												startI, startJ;

											if (map->isWithinBuildRange(location->x, location->y, this))
												startI = location->x, startJ = location->y, incI = 1, incJ = 1;
											else if (map->isWithinBuildRange(location->x + item->getStructureSizeX() - 1, location->y, this))
												startI = location->x + item->getStructureSizeX() - 1, startJ = location->y, incI = -1, incJ = 1;
											else if (map->isWithinBuildRange(location->x, location->y + item->getStructureSizeY() - 1, this))
												startI = location->x, startJ = location->y + item->getStructureSizeY() - 1, incI = 1, incJ = -1;
											else
												startI = location->x + item->getStructureSizeX() - 1, startJ = location->y + item->getStructureSizeY() - 1, incI = -1, incJ = -1;

											for (i = startI; abs(i - startI) < item->getStructureSizeX(); i += incI)
											for (j = startJ; abs(j - startJ) < item->getStructureSizeY(); j += incJ)
											if (map->getCell(i, j)->getType() != Structure_Slab1)
											{
												location = new COORDTYPE;
												location->x = i;
												location->y = j;
												placeLocations.insertLast(location);
												builder->buildType(Structure_Slab1);
											}
										}

										location = new COORDTYPE;
										location->x = placeLocation.x;
										location->y = placeLocation.y;
										placeLocations.insertLast(location);
										builder->buildType(itemID);
									}

									delete item;
								}
							}
						}
						
						if (builder->isWaitingToPlace(NONE))
						{	//find total region of possible placement and place in random ok position
							BuildItemClass* item = builder->getPlaceItem();

							//see if there is already a spot to put it stored
							if (!placeLocations.isEmpty())
							{
								bool concreteRequired = settings.concreteRequired && (item->getItemID() != Structure_Slab1);
								COORDTYPE* location = (COORDTYPE*)placeLocations.getFirstElement();

								if (map->okayToPlaceStructure(location->x, location->y, item->getStructureSizeX(), item->getStructureSizeY(), concreteRequired, builder->getOwner()))
								{
									builder->getOwner()->netPlaceStructure(builder->getObjectID(), builder->getX(), builder->getY(), item->getItemID(), location->x, location->y);
									placeLocations.removeFirstElement();
									delete location;
								}
								else if (item->getItemID() == Structure_Slab1)
								{
									builder->cancelSelection(item);	//forget about concrete
									builder->cancelSelection(item);
									if (item->getNumToBeBuilt() > 0)
										item->setOnHold(false);
									placeLocations.removeFirstElement();
									delete location;
								}
								else
								{	//put the item to back of list for trying later
									COORDTYPE	*placeLocation;

									placeLocation = (COORDTYPE*)placeLocations.removeFirstElement();

									builder->cancelSelection(item);	//cancel twice to cancel completed building
									builder->cancelSelection(item);
									if ((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) 
									{
										if (settings.concreteRequired)
										{
											int i, j;
											COORDTYPE* temp;

											for (i = placeLocation->x; i < placeLocation->x + item->getStructureSizeX(); i++)
											for (j = placeLocation->y; j < placeLocation->y + item->getStructureSizeY(); j++)
												if (map->getCell(i, j)->getType() != Structure_Slab1)
												{
													temp = new COORDTYPE;
													temp->x = i;
													temp->y = j;
													placeLocations.insertLast(temp);
													builder->buildType(Structure_Slab1);
												}
										}
										builder->buildType(item->getItemID());
										placeLocations.insertLast(placeLocation);
									}
									else
										delete placeLocation;
								}
							}
						}
						break;
					case (Structure_HeavyFactory):
						if (((credits + startingCredits) > 100) && (builder->getNumItemsToBuild() < 1) && (builder->getNumSelections() > 0))
						{
							if (numHarvesters < 2*numRefineries)
								builder->buildType(Unit_Harvester);
							else
								builder->buildRandom();
						}
						break;
					case (Structure_HighTechFactory):
						if (((credits + startingCredits) > 100) && (builder->getNumItemsToBuild() < 1))
						{
							if (numCarryalls < numHarvesters)
								builder->buildType(Unit_Carryall);
							else
								builder->buildType(Unit_Ornithopter);
						}
						break;
					case (Structure_StarPort):
						if (((StarPortClass*)builder)->okToOrder())
						{
							while ((credits + startingCredits) > 2000)
								builder->buildRandom();
							((StarPortClass*)builder)->placeOrder();
						}
						break;
					default:
						break;
					}
				}
				structureList->nextLink();
			}
		}

		buildTimer = getRandomInt(100, 200);
	}

	/*char temp[10];
	sprintf(temp, "at is %d", attackTimer);
	showMessage(temp);*/
	if (attackTimer > 0) attackTimer--;
	else
	{
		if (!unitList->isEmpty())
		{
			double speedCap = NONE;
			COORDTYPE	destination;
			UnitClass	*leader = NULL,
						*tempUnit;

			unitList->reset();
			while(unitList->currentNotNull())
			{
				tempUnit = (UnitClass*)unitList->getCurrent();
				if (tempUnit->isRespondable()
					&& (tempUnit->getOwner() == this)
					&& !tempUnit->isAttacking()
					&& (tempUnit->getAttackMode() == AGGRESSIVE)
					&& (tempUnit->getItemID() != Unit_Harvester)
					&& (tempUnit->getItemID() != Unit_MCV)
					&& (tempUnit->getItemID() != Unit_Carryall)
					&& (tempUnit->getItemID() != Unit_Saboteur))
				{
					//find slowest speed of all units
					if ((speedCap < 0) || (tempUnit->getMaxSpeed() < speedCap))
						speedCap = tempUnit->getMaxSpeed();
				}
				unitList->nextLink();
			}

			unitList->reset();
			while(unitList->currentNotNull())
			{
				tempUnit = (UnitClass*)unitList->getCurrent();
				if (tempUnit->isRespondable()
					&& (tempUnit->getOwner() == this)
					&& tempUnit->isActive()
					&& !tempUnit->isAttacking()
					&& (tempUnit->getAttackMode() == AGGRESSIVE)
					&& (tempUnit->getItemID() != Unit_Harvester)
					&& (tempUnit->getItemID() != Unit_MCV)
					&& (tempUnit->getItemID() != Unit_Carryall)
					&& (tempUnit->getItemID() != Unit_Saboteur))
				{
					if (!leader)
					{
						leader = tempUnit;

						//default destination
						destination.x = leader->getX();
						destination.y = leader->getY();

						StructureClass* closestStructure = (StructureClass*)ObjectClass::findClosestTargetStructure(leader);
						if (closestStructure)
							destination = closestStructure->getClosestPoint(leader->getLocation());
						else
						{
							UnitClass* closestUnit = (UnitClass*)ObjectClass::findClosestTargetUnit(leader);
							if (closestUnit)
							{
								destination.x = closestUnit->getX();
								destination.y = closestUnit->getY();
							}
						}
					}

					tempUnit->setSpeedCap(speedCap);
					tempUnit->netDoCommand(NULL, &destination, false);
					tempUnit->setAttacking(true);
				}
				unitList->nextLink();
			}
		}	

		//reset timer for next attack
		attackTimer = getRandomInt(10000, 20000);
	}
	if (buildTimer > 0) buildTimer--;
}

bool AiPlayerClass::isAttacking()
{
	return ((settings.playerType != CLIENT) && (attackTimer < 0));
}

COORDTYPE* AiPlayerClass::findPlaceLocation(int structureSizeX, int structureSizeY)
{
	static COORDTYPE bestLocation;
	int	count, x, y,
		minX = map->sizeX, maxX = -1,
		minY = map->sizeY, maxY = -1;
	StructureClass* structure;

	bestLocation.x = bestLocation.y = NONE;

	structureList->saveCurrent();
	structureList->reset();
	while(structureList->currentNotNull())
	{
		structure = (StructureClass*)structureList->getCurrent();
		if (structure->getOwner() == this)
		{
			if (structure->getX() < minX)
				minX = structure->getX();
			if (structure->getX() > maxX)
				maxX = structure->getX();
			if (structure->getY() < minY)
				minY = structure->getY();
			if (structure->getY() > maxY)
				maxY = structure->getY();
		}

		structureList->nextLink();
	}
	structureList->restoreCurrent();

	minX -= structureSizeX + 1;
	maxX += 2;
	minY -= structureSizeY + 1;
	maxY += 2;
	if (minX < 0) minX = 0;
	if (maxX >= map->sizeX) maxX = map->sizeX - 1;
	if (minY < 0) minY = 0;
	if (maxY >= map->sizeX) maxY = map->sizeX - 1;

	count = 0;
	do
	{
		x = getRandomInt(minX, maxX);
		y = getRandomInt(minY, maxY);
		count++;
	} while (!map->okayToPlaceStructure(x, y, structureSizeX, structureSizeY, false, this) && (count <= 100));
	if (count <= 100)
	{	//a suitable location found!!!
		bestLocation.x = x;
		bestLocation.y = y;
	}

	return &bestLocation;
}

