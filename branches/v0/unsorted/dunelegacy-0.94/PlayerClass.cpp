#include "sand.h"
#include "CommViewClass.h"
#include "Items.h"

PlayerClass::PlayerClass(int newPlayerNumber, int newHouse, int newColour, int newCredits)
{
	playerNumber = newPlayerNumber;
	mapPlayerNum = NONE;
	if ((newHouse >= 0) && (newHouse < NUM_HOUSES))
		house = newHouse;
	else
		house = 0;
	if ((newColour >= 0) && (newColour < NUM_HOUSES))
		colour = newColour;
	else
		colour = 0;

	ai = false;
	credits = 0;
	creditsCounter = 0;
	capacity = 0;
	power = 0;
	powerRequirement = 0;
	strcpy(name, settings.playerName[getPlayerNumber()]);
	startingCredits = newCredits;
	team = newPlayerNumber;

	numBarracks = 0;

	numCarryalls = 0;
	numHarvesters = 0;
	numHeavyFactories = 0;
	numHighTechFactories = 0;
	numIXs = 0;
	numLightFactories = 0;
	numPalaces = 0;
	numRadars = 0;
	numRefineries = 0;
	numRepairYards = 0;
	numRocketTurrets = 0;
	numStarPorts = 0;
	numStructures = 0;
	numWalls = 0;
	numWindTraps = 0;
	numWORs = 0;
	numUnits = 0;



	quitingGame = false;

	oldCredits = getAmountOfCredits();

}
PlayerClass::~PlayerClass()
{
}
void PlayerClass::addCredits(double newCredits)
{
	if (newCredits > 0.0)
	{
		double	oldCredits = credits;


		credits += newCredits;

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

void PlayerClass::assignMapPlayerNum(int newMapPlayerNum)
{
	if ((newMapPlayerNum >=0) && (newMapPlayerNum < MAX_PLAYERS))
		mapPlayerNum = newMapPlayerNum;
	else
		mapPlayerNum = 0;
}

void PlayerClass::checkSelectionLists()
{
	if (!structureList->isEmpty())

	{

		StructureClass* tempStructure;



		structureList->saveCurrent();

		structureList->reset();
		while(structureList->currentNotNull())
		{
			tempStructure = (StructureClass*)structureList->getCurrent();
			if (tempStructure->isABuilder() && (tempStructure->getOwner() == this))
				((BuilderClass*)tempStructure)->checkSelectionList();
			structureList->nextLink();
		}

		structureList->restoreCurrent();

	}
}



void PlayerClass::decrementStructures(int itemID, COORDTYPE* location)
{
	numStructures--;
	switch (itemID)

	{
	case (Structure_Barracks):
		numBarracks--;
		break;
	case (Structure_ConstructionYard):
		//numConstructionYards--;
		break;
	case (Structure_GunTurret):
		//numGunTurrets--;
		break;
	case (Structure_HeavyFactory):
		numHeavyFactories--;
		break;
	case (Structure_HighTechFactory):
		numHighTechFactories--;
		break;
	case (Structure_IX):
		numIXs--;
		break;
	case (Structure_LightFactory):
		numLightFactories--;
		break;
	case (Structure_Palace):
		numPalaces--;
		break;
	case (Structure_Radar):
		numRadars--;
		break;
	case (Structure_Refinery):

	{

		numRefineries--;
		removeCapacity(REFINERY_CAPACITY);
		break;
	}
	case (Structure_RepairYard):
		numRepairYards--;
		break;
	case (Structure_RocketTurret):
		numRocketTurrets--;
		break;
	case (Structure_Silo):
//		numSilos--;
		removeCapacity(SILO_CAPACITY);
		break;
	case (Structure_StarPort):
		numStarPorts--;
		break;
	case (Structure_Wall):
		numWalls--;
		break;
	case (Structure_WindTrap):
		numWindTraps--;
		removePower();
		break;
	case (Structure_WOR):
		numWORs--;
		break;
	default:
		break;
	}



	/*char temp[20];

	sprintf(temp, "numStructures:%d", numStructures);

	showMessage(temp);*/



	checkSelectionLists();



	if (!isAlive())

		lose();
}



void PlayerClass::decrementHarvesters()

{

	--numHarvesters;

	if (!quitingGame)

	{

		if (numHarvesters <= 0)

		{

			numHarvesters = 0;



			if (numRefineries)

			{

				COORDTYPE	closestPos,

					pos = {0,0};



				if (!structureList->isEmpty())

				{

					double	closestDistance,

							refineryDistance;

					StructureClass	*closestRefinery = NULL,

									*tempStructure;



					structureList->reset();

					while(structureList->currentNotNull())

					{

						tempStructure = (StructureClass*)structureList->getCurrent();

						if ((tempStructure->getItemID() == Structure_Refinery) && (tempStructure->getOwner() == this))

						{

							pos = *tempStructure->getLocation();



							refineryDistance = blockDistance(&pos, &tempStructure->getClosestPoint(&pos));

							if (!closestRefinery || (refineryDistance < closestDistance))

							{

								closestDistance = refineryDistance;

								closestRefinery = tempStructure;

								closestPos = pos;

							}

						}

						

						structureList->nextLink();

					}



					if (closestRefinery && (gameState == BEGUN))

						freeHarvester(closestRefinery->getLocation()->x, closestRefinery->getLocation()->y);

				}

			}

		}

	}

}

void PlayerClass::decrementUnits(int itemID)
{
	if (itemID != Unit_Sandworm)
		numUnits--;
	switch (itemID)

	{

	case (Unit_Carryall):

		numCarryalls--;

		break;

	case (Unit_Harvester):

		decrementHarvesters();

		break;

	default:

		break;

	}



	if (!isAlive())

		lose();

	/*char temp[20];

	sprintf(temp, "numUnits:%d", numUnits);
	showMessage(temp);*/
}

void PlayerClass::freeHarvester(int xPos, int yPos)
{
	if (map->cellExists(xPos, yPos)
		&& map->getCell(xPos, yPos)->hasAGroundObject()
		&& (map->getCell(xPos, yPos)->getGroundObject()->getItemID() == Structure_Refinery))
	{
		RefineryClass* refinery = (RefineryClass*)map->getCell(xPos, yPos)->getGroundObject();
		COORDTYPE closestPos = map->findClosestEdgePoint(refinery->getLocation(), refinery->getStructureSize());

		Carryall* carryall = (Carryall*)createUnit(Unit_Carryall);

		HarvesterClass* harvester = (HarvesterClass*)createUnit(Unit_Harvester);

		carryall->setOwned(false);

		carryall->giveCargo(harvester);

		carryall->deploy(&closestPos);

		carryall->setDropOfferer(true);



		if (closestPos.x == 0)

			carryall->setAngle(RIGHT);

		else if (closestPos.x == map->sizeX-1)

			carryall->setAngle(LEFT);

		else if (closestPos.y == 0)

			carryall->setAngle(DOWN);

		else if (closestPos.y == map->sizeY-1)

			carryall->setAngle(UP);



		harvester->setTarget(refinery);

		harvester->setActive(false);

		carryall->setTarget(refinery);
	}
}



void PlayerClass::load(FILE* fp)

{

	int		j;

	double	tempDouble;



	fread(&j, sizeof(int), 1, fp);
	setTeam(j);
	fread(&j, sizeof(int), 1, fp);
	if (j)
		thisPlayer = this;
	fread(&tempDouble, sizeof(double), 1, fp);
	setStartingCredits(tempDouble);
	fread(&tempDouble, sizeof(double), 1, fp);
	setStoredCredits(tempDouble);

}



void PlayerClass::lose()

{

	char message[100];

	sprintf(message, "%s has been defeated.", name);

	showMessage(message);



	if (this == thisPlayer)

	{

		settings.finished = true;

		if (settings.playerType == SINGLE)

			settings.update = false;

		settings.won = false;



		soundPlayer->changeMusic(MUSIC_LOSE);

		switch (house)
		{
			case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
				soundPlayer->playVOSound(SoundVO_ALose);
				break;
			case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
				soundPlayer->playVOSound(SoundVO_OLose);
				break;
			case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
				soundPlayer->playVOSound(SoundVO_HLose);
				break;
			default:
				break;
		}

	}

	else if (settings.winFlags != 6)

	{

		//if the only players left are on the thisPlayers team, thisPlayer has one

		bool finished = true;

		int	lastTeam = NONE;



		for (int i=0; i<NUM_HOUSES; i++)

		{

			if ((player[i] != NULL) && player[i]->isAlive() && (player[i]->getTeam() != thisPlayer->getTeam()))

				finished = false;

		}



		if (finished)

		{

			settings.finished = true;

			if (settings.playerType == SINGLE)

				settings.update = false;

			/*always won can cause fault if next mission should load, 
			but game was aborted*/
			
			/*settings.won = true;*/

			soundPlayer->changeMusic(MUSIC_WIN);

			switch (house)
			{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					soundPlayer->playVOSound(SoundVO_AWin);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					soundPlayer->playVOSound(SoundVO_OWin);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					soundPlayer->playVOSound(SoundVO_HWin);
					break;
				default:
					break;
			}

		}

	}

}



void PlayerClass::netPlaceStructure(int builderID, int x, int y, int itemID, int xPos, int yPos)

{

	StructureClass* structure = NULL;

	if (settings.playerType == SINGLE)
		structure = (StructureClass*)placeStructure(builderID, x, y, itemID, xPos, yPos);
	else if (settings.playerType == SERVER)
	{
		structure = (StructureClass*)placeStructure(builderID, x, y, itemID, xPos, yPos);
		if (builderID == NONE)
			netServerPlaceStructure(getPlayerNumber(), builderID, x, y, NULL, NULL, NULL, NULL);
		else
			netServerPlaceStructure(getPlayerNumber(), builderID, x, y, itemID, xPos, yPos, NULL);
	}
	else if (builderID == NONE)
		netClientPlaceStructure(getPlayerNumber(), builderID, x, y, NULL, NULL, NULL, NULL);
	else
		netClientPlaceStructure(getPlayerNumber(), builderID, x, y, itemID, xPos, yPos, NULL);



	if (structure)

		structure->setJustPlaced();

}



void PlayerClass::noteDamageLocation(void* object, COORDTYPE* location)

{

	//if (this == thisPlayer)

	//	soundPlayer->playVOSound(Sound

}


void PlayerClass::returnCredits(double newCredits)

{

	if (newCredits > 0.0)

	{

		double	oldCredits = startingCredits;

		startingCredits += newCredits;

	}

}

void PlayerClass::sapPower(int amount)

{

	bool powerOff = !hasPower();



	if (hasRadarOn())

	{

		powerRequirement += amount;

	

		if (!hasPower() && (this == thisPlayer))

		{

			changeRadar(false);

		}

	}

	else

		powerRequirement += amount;

}

void PlayerClass::save(FILE* fp)
{
	int		j;
	double	tempDouble;

	j = getHouse();
	fwrite(&j, sizeof(int), 1, fp);
	j = getColour();
	fwrite(&j, sizeof(int), 1, fp);
	j = 0;	//not an ai player
	fwrite(&j, sizeof(int), 1, fp);
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

void PlayerClass::setStartingCredits(double newCredits)
{
	if (newCredits >= 0.0)
		startingCredits = newCredits;
}

void PlayerClass::setStoredCredits(double newCredits)
{
	if (newCredits >= 0.0)
		credits = newCredits;
}

void PlayerClass::update()
{
	if (oldCredits != getAmountOfCredits())
	{
		if ((this == thisPlayer) && (getAmountOfCredits() > 0))
			soundPlayer->playSfxSound(Sound_credit);
		oldCredits = getAmountOfCredits();
	}

	if (credits > capacity)
	{
		credits--;// = capacity;
		if (this == thisPlayer)
			showMessage("spice lost, build more silos.");
	}
}



void PlayerClass::unsapPower(int amount)

{

	if (!hasPower())

	{

		powerRequirement -= amount;

		if (hasRadarOn())

		{

			changeRadar(true);

		}

	}

	else

		powerRequirement -= amount;

}



void PlayerClass::win()

{

	settings.finished = true;

	if (settings.playerType == SINGLE)

		settings.update = false;

	if (getTeam() == thisPlayer->getTeam())

	{

		settings.won = true;



		soundPlayer->changeMusic(MUSIC_WIN);

		switch (house)
		{
			case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
				soundPlayer->playVOSound(SoundVO_AWin);
				break;
			case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
				soundPlayer->playVOSound(SoundVO_OWin);
				break;
			case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
				soundPlayer->playVOSound(SoundVO_HWin);
				break;
			default:
				break;
		}

	}

	else

	{

		settings.won = false;



		soundPlayer->changeMusic(MUSIC_LOSE);

		switch (house)
		{
			case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
				soundPlayer->playVOSound(SoundVO_ALose);
				break;
			case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
				soundPlayer->playVOSound(SoundVO_OLose);
				break;
			case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
				soundPlayer->playVOSound(SoundVO_HLose);
				break;
			default:
				break;
		}

	}

}

void* PlayerClass::placeStructure(int builderID, int x, int y, int itemID, int xPos, int yPos)
{
	numStructures++;
	

	StructureClass* tempStructure = NULL;
	switch (itemID)
	{
	case (Structure_Barracks):
		tempStructure = new BarracksClass(this);
		break;
	case (Structure_ConstructionYard):
		tempStructure = new ConstructionYardClass(this);
		break;
	case (Structure_GunTurret):
		tempStructure = new GunTurretClass(this);
		break;
	case (Structure_HeavyFactory):
		tempStructure = new HeavyFactoryClass(this);
		numHeavyFactories++;
		break;
	case (Structure_HighTechFactory):
		tempStructure = new HighTechFactoryClass(this);
		numHighTechFactories++;
		break;
	case (Structure_IX):
		tempStructure = new IXClass(this);
		numIXs++;
		break;
	case (Structure_LightFactory):
		tempStructure = new LightFactoryClass(this);
		numLightFactories++;
		break;
	case (Structure_Palace):
		tempStructure = new PalaceClass(this);
		numPalaces++;
		break;
	case (Structure_Radar):
		tempStructure = new RadarClass(this);
		numRadars++;
		break;
	case (Structure_Refinery):

	{

		tempStructure = new RefineryClass(this);
		numRefineries++;
		addCapacity(REFINERY_CAPACITY);
		break;

	}
	case (Structure_RepairYard):
		tempStructure = new RepairYardClass(this);
		numRepairYards++;
		break;
	case (Structure_RocketTurret):
		tempStructure = new RocketTurretClass(this);
		numRocketTurrets++;
		break;
	case (Structure_Silo):
		tempStructure = new SiloClass(this);
		addCapacity(SILO_CAPACITY);
		break;
	case (Structure_StarPort):
		tempStructure = new StarPortClass(this);
		numStarPorts++;
		break;
	case (Structure_Slab1):
		map->getCell(xPos, yPos)->setType(Structure_Slab1);
		map->getCell(xPos, yPos)->setOwner(getPlayerNumber());
		map->viewMap(getTeam(), xPos, yPos, TILE_VIEWRANGE);
//		map->getCell(xPos, yPos)->clearDamage();

		if (builderID != NONE)
		{
			ObjectClass*	object = map->findObjectWidthID(builderID, x, y);
			if (object->isAStructure() && ((StructureClass*)object)->isABuilder())
			{
			
				((BuilderClass*)object)->unSetWaitingToPlace();
				if (this == thisPlayer)
					placingMode = false;
			}
		}
		else if (this == thisPlayer)
			placingMode = false;

		numStructures--;
		break;
	case (Structure_Slab4):
	{
		int i,j;
		for(i = xPos; i < xPos + 2; i++)
		for(j = yPos; j < yPos + 2; j++)
		if (map->cellExists(i, j))
		{
			TerrainClass* cell = map->getCell(i, j);

			if (!cell->hasAGroundObject() && cell->isRock() && !cell->isMountain())
			{
				cell->setType(Structure_Slab1);
				cell->setOwner(getPlayerNumber());
				map->viewMap(getTeam(), i, j, TILE_VIEWRANGE);
				//cell->clearDamage();
			}
		}

		if (builderID != NONE)
		{
			ObjectClass*	object = map->findObjectWidthID(builderID, x, y);
			if (object->isAStructure() && ((StructureClass*)object)->isABuilder())
			{
			
				((BuilderClass*)object)->unSetWaitingToPlace();
				if (this == thisPlayer)
					placingMode = false;
			}
		}
		else if (this == thisPlayer)
			placingMode = false;

		numStructures--;
		break;
	}
	case (Structure_Wall):
		tempStructure = new WallClass(this);

		numWalls++;
		break;
	case (Structure_WindTrap):
	{
		bool powerOff = !hasPower();
		tempStructure = new WindTrapClass(this);
		addPower();
		if (powerOff && hasRadarOn() && (this == thisPlayer))

		{

				changeRadar(true);
		}
		numWindTraps++;
		break;
	}
	case (Structure_WOR):
		tempStructure = new WORClass(this);
		numWORs++;
		break;
	default:
		numStructures--;
		break;
	}



	if (tempStructure)

	{

		

		//char temp[50];

		//sprintf(temp, "x: %d, y: %d", xPos, yPos);

		//showMessage(temp);

		

		for(int i=0;i<tempStructure->getStructureSizeX();i++)

		{

			for(int j=0;j<tempStructure->getStructureSizeY();j++)

			{

				//sprintf(temp, "clear x: %d, y: %d", xPos + i, yPos + j);

				//showMessage(temp);

				if (map->cellExists(xPos+i, yPos+j))
				{
					map->getCell(xPos+i, yPos+j)->clearDamage();

				}

			}

		}



		tempStructure->setLocation(xPos, yPos);

		if (itemID == Structure_Wall)

			fixWalls(xPos, yPos);

		structureList->insertLast(tempStructure);

		if ((gameState == BEGUN) && (itemID == Structure_Refinery))
			freeHarvester(xPos, yPos);
		
		if (builderID != NONE)
		{
			ObjectClass*	object = map->findObjectWidthID(builderID, x, y);
			if (object->isAStructure() && ((StructureClass*)object)->isABuilder())
			{	
				((BuilderClass*)object)->unSetWaitingToPlace();
				if (this == thisPlayer)
					placingMode = false;
			}
		}
		else if (this == thisPlayer)
			placingMode = false;

		sapPower(tempStructure->getPowerRequirement());

		if(itemID == Structure_Radar && hasPower())
		{
			changeRadar(true);
		}

		checkSelectionLists();

		/*char temp[20];

		sprintf(temp, "numStructures:%d", numStructures);

		showMessage(temp);*/
	}

	return tempStructure;
}

void* PlayerClass::createUnit(int itemID)

{

	UnitClass* newUnit = NULL;
	switch (itemID)
	{

	case (Unit_Carryall):

		newUnit = new Carryall(this);

		numCarryalls++;

		break;
	case (Unit_Devastator):
		newUnit = new DevastatorClass(this);
		break;
	case (Unit_Deviator):
		newUnit = new DeviatorClass(this);
		break;
	case (Unit_Frigate):
		newUnit = new Frigate(this);
		break;
	case (Unit_Harvester):
		newUnit = new HarvesterClass(this);
		numHarvesters++;
		break;
	case (Unit_Infantry):
		newUnit = new InfantryClass(this);
		break;
	case (Unit_Launcher):
		newUnit = new LauncherClass(this);
		break;
	case (Unit_MCV):
		newUnit = new MCVClass(this);
		break;

	case (Unit_Ornithopter):

		newUnit = new Ornithopter(this);

		break;
	case (Unit_Quad):
		newUnit = new QuadClass(this);
		break;
	case (Unit_Saboteur):
		newUnit = new Saboteur(this);
		break;
	case (Unit_Sandworm):
		newUnit = new Sandworm(this);
		break;
	case (Unit_SiegeTank):
		newUnit = new SiegeTankClass(this);
		break;
	case (Unit_SonicTank):
		newUnit = new SonicTankClass(this);
		break;
	case (Unit_Tank):
		newUnit = new TankClass(this);
		break;
	case (Unit_Trike):
		newUnit = new TrikeClass(this);
		break;
	case (Unit_Raider):
		newUnit = new RaiderClass(this);
		break;
	case (Unit_Trooper):
		newUnit = new TrooperClass(this);
		break;
	case (Unit_Sardaukar):
		newUnit = new SardaukarClass(this);
		break;
	case (Unit_Fremen):
		newUnit = new FremenClass(this);
		break;
	}



	if (newUnit)
	{
		unitList->insertFirst(newUnit);
		if (itemID != Unit_Sandworm)
			numUnits++;

		/*char temp[20];

		sprintf(temp, "numUnits:%d", numUnits);

		showMessage(temp);*/
	}

	return newUnit;
}

void* PlayerClass::placeUnit(int itemID, int xPos, int yPos)

{

	UnitClass* newUnit = NULL;
	if (map->cellExists(xPos, yPos))
		newUnit = (UnitClass*)createUnit(itemID);



	if (newUnit)
	{
		COORDTYPE pos = {xPos, yPos};
		if (newUnit->canPass(xPos, yPos))
			newUnit->deploy(&pos);
		else
		{
			newUnit->setVisible(VIS_ALL, false);
			newUnit->destroy();
			newUnit = NULL;
		}
	}

	return newUnit;
}

int PlayerClass::getMapPlayerNum()
{
	return mapPlayerNum;
}

double PlayerClass::takeCredits(double amount)
{
	double	oldCredits = credits + startingCredits,
			taken = 0.0;	//take a record of the old amount

	if (oldCredits >= 1)
	{
		if (credits >= amount)
		{
			taken = amount;
			credits -= amount;
		}
		else
		{
			taken = credits;
			credits = 0.0;

			if (startingCredits >= (amount - taken))
			{
				startingCredits -= (amount - taken);
				taken = amount;
			}
			else
			{
				taken += startingCredits;
				startingCredits = 0.0;
			}
		}
	}

	return taken;	//the amount that was actually withdrawn
}

void PlayerClass::changeRadar(bool status)
{
	if (this == thisPlayer)
	{
		soundPlayer->playVOSound(Sound_radar);
		if(status)
		{
			//open animation
		    extern CommViewClass *commView;
            if (commView) delete commView;
			commView = CommViewClass::getCommViewClass(AnimationRadarOn);
			
			//turn on
			switch (this->getHouse())
			{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					soundPlayer->playVOSoundDual(SoundVO_Aradar, SoundVO_Aon);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					soundPlayer->playVOSoundDual(SoundVO_Oradar, SoundVO_Oon);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					soundPlayer->playVOSoundDual(SoundVO_Hradar, SoundVO_Hon);
					break;
				default:
					break;
			}
		}
		else
		{
			//open animation
		    extern CommViewClass *commView;
            if (commView) delete commView;
			commView = CommViewClass::getCommViewClass(AnimationRadarOff);
			
			//turn off
			switch (this->getHouse())
			{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					soundPlayer->playVOSoundDual(SoundVO_Aradar, SoundVO_Aoff);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					soundPlayer->playVOSoundDual(SoundVO_Oradar, SoundVO_Ooff);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					soundPlayer->playVOSoundDual(SoundVO_Hradar, SoundVO_Hoff);
					break;
				default:
					break;
			}
		}
	}
}

