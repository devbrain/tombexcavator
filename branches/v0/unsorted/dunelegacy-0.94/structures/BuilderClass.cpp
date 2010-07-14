#include "../sand.h"

#include "../Items.h"

SDL_Surface *RepairUp;
SDL_Surface *RepairDown;

SDL_Surface *UpgradeUp;
SDL_Surface *UpgradeDown;

BuilderClass::BuilderClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	selectionList = new ListIterator();	//contains things u can build
	buildList = new ListIterator();		//contains pointers to things in the selectionList that u will be building

	minSelection = 0;
	maxSelection = -1;
	priceModifier = 1.0;

	aBuilder = true;

	currentItem = NULL;

	upgradeLevels = upgradeCost = curUpgradeLev = upgradeProgress = lastUpgradeProgress = 0;
	upgrading = false;

	setupButtons();
}

void BuilderClass::setOwner(PlayerClass *no)
{
	this->owner = no;
	setupButtons();
}

void BuilderClass::setupButtons()
{
	if(this->owner == thisPlayer)//only setup boxes for player stuff
	{
		if(interfaceContainer)
			delete interfaceContainer;

		interfaceContainer = new Container();

		interfaceContainer->setX(gameBarPos.x + BUILDERBUTTONSMINX - 17);//gameBarPos.x + 29);
		//interfaceContainer->setX(gameBarPos.x + 29);
		interfaceContainer->setY(16 + 128);
		interfaceContainer->setWidth(96);//(screen->w - 16) - interfaceContainer->getX());
		interfaceContainer->setHeight(screen->h - interfaceContainer->getY());

		/*SDL_Surface * */RepairUp = copySurface((SDL_Surface*)(dataFile[UI_Repair].dat));
		/*SDL_Surface * */RepairDown = copySurface((SDL_Surface*)(dataFile[UI_Repair_Pressed].dat));

		repairButton.setSurface(RepairUp, NULL);
		repairButton.setPressedSurface(RepairDown, NULL);

		/*repairButton.setText("Repair");*/
		repairButton.setX(interfaceContainer->getX());
		repairButton.setY(interfaceContainer->getY());
		repairButton.setHeight(RepairUp->h);
		repairButton.setWidth(RepairUp->w);
		repairButton.setCallBack(repairCallback, this);
		repairButton.setVisible(false);

		/*load surfaces of UpgradeButton*/
		UpgradeUp = copySurface((SDL_Surface*)(dataFile[UI_Upgrade].dat));
		UpgradeDown = copySurface((SDL_Surface*)(dataFile[UI_Upgrade_Pressed].dat));
		mapImageHouseColour(UpgradeUp,thisPlayer->getColour());
		mapImageHouseColour(UpgradeDown,thisPlayer->getColour());
		
		interfaceContainer->addChild(&repairButton);
	

		VBox* buttonsBox = new VBox();
		buttonsBox->setBuffering(9);
		buttonsBox->setX(gameBarPos.x + BUILDERBUTTONSMINX - 1);
		buttonsBox->setY(BUILDERBUTTONSMINY - 1);
		buttonsBox->setWidth(65);
		buttonsBox->setHeight(223);
		interfaceContainer->addChild(buttonsBox);
		buttonsBox->setVisible(true);

		b1.setEnabled(false);
		b2.setEnabled(false);
		b3.setEnabled(false);
		b4.setEnabled(false);
		b1.setVisible(false);
		b2.setVisible(false);
		b3.setVisible(false);
		b4.setVisible(false);

		data1.object = data2.object = data3.object = data4.object = this;

		interfaceContainer->addChild(&up);
		buttonsBox->addChild(&b1);
		buttonsBox->addChild(&b2);
		buttonsBox->addChild(&b3);
		buttonsBox->addChild(&b4);
		interfaceContainer->addChild(&down);

		SDL_Surface *picup = copySurface((SDL_Surface*)(dataFile[UI_Up].dat));
		SDL_Surface *picdown = copySurface((SDL_Surface*)(dataFile[UI_Down].dat));
		SDL_Surface *picuppress	= copySurface((SDL_Surface*)(dataFile[UI_Up_Pressed].dat));
		SDL_Surface *picdownpress = copySurface((SDL_Surface*)(dataFile[UI_Down_Pressed].dat));

		mapImageHouseColour(picup, thisPlayer->getColour());
		mapImageHouseColour(picdown, thisPlayer->getColour());

		//up.setText("up");
		up.setSurface(picup, NULL);
		up.setPressedSurface(picuppress , NULL);
		up.setCallBack(buttonUp, this);
		//up.setX(gameBarPos.x + BUILDERBUTTONSMINX - 17);
		up.setX(gameBarPos.x + BUILDERBUTTONSMINX + 7);
		up.setY(BUILDERBUTTONSMINY - 36);
		up.setWidth(picup->w);
		up.setHeight(picup->h);
		up.setVisible(true);

		//down.setText("down");
		down.setSurface(picdown, NULL);
		down.setPressedSurface(picdownpress , NULL);
		down.setCallBack(buttonDown, this);
		//down.setX(gameBarPos.x + BUILDERBUTTONSMINX - 17);
		down.setX(gameBarPos.x + BUILDERBUTTONSMINX + 7);
		down.setY(BUILDERBUTTONSMAXY + 15);
		down.setWidth(picdown->w);
		down.setHeight(picup->h);
		down.setVisible(true);

		interfaceContainer->setVisible(true);
	}
}


BuilderClass::~BuilderClass()
{
	selectionList->reset();
	while(selectionList->currentNotNull())
		delete (reinterpret_cast<BuildItemClass*>(selectionList->removeCurrent()));
	delete selectionList;
	delete buildList;

	delete interfaceContainer;
	//delete buttonsBox;
}

void BuilderClass::buildUpdate()
{
	if (currentItem)
	{
		if (!currentItem->isWaitingToPlace())
		{
			if ((owner->getAmountOfCredits() > 0) && !currentItem->isOnHold() && (currentItem->getPercentComplete() < 100.0))
			{
				double oldProgress = currentItem->getProgress();
				if (getOwner()->hasPower() || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && getOwner()->isAI()))	//if not enough power, production is halved
					currentItem->buildUpdate(owner->takeCredits(0.5));
				else
					currentItem->buildUpdate(owner->takeCredits(0.25));

				if ((oldProgress == currentItem->getProgress())
					&& (owner == thisPlayer))
					showMessage("not enough credits");

				if (currentItem->getProgress() >= currentItem->getCost())
					netSetWaitingToPlace();
			}
		}
	}
}



void BuilderClass::checkMinMaxSelection()
{
	bool			found;
	int				count;
	BuildItemClass	*buildItem,
					*selectionItem;

	count = 0;
	buildList->reset();
	while(buildList->currentNotNull())
	{
		buildItem = (BuildItemClass*)buildList->getCurrent();
		found = false;
		selectionList->reset();
		while(selectionList->currentNotNull() && !found)
		{
			selectionItem = (BuildItemClass*)selectionList->getCurrent();
			if (buildItem == selectionItem)
				found = true;
			selectionList->nextLink();
		}

		if (!found)
		{
			//remove all links to it
			while (buildList->removeElement(buildItem))
				found = false;	//just to stop complaints
			if (buildItem == currentItem)
			{
				getOwner()->returnCredits(currentItem->getProgress());
				currentItem = NULL;
			}
			delete buildItem;
			if (owner->isAI())
				((AiPlayerClass*)getOwner())->clearPlaceList(count);
		}
		else
		{
			buildList->nextLink();
			count++;
		}
	}

	if (currentItem)
	{
		found = false;
		selectionList->reset();
		while(selectionList->currentNotNull() && !found)
		{
			selectionItem = (BuildItemClass*)selectionList->getCurrent();
			if (currentItem == selectionItem)
				found = true;
			selectionList->nextLink();
		}

		if (!found)
		{
			while (buildList->removeElement(currentItem))	//remove all links to it
				found = false;	//just to stop complaints
			getOwner()->returnCredits(currentItem->getProgress());
			delete currentItem;
			currentItem = NULL;
		}
	}

	if ((currentItem == NULL) && (buildList->getNumElements() > 0))
	{
		currentItem = (BuildItemClass*)buildList->getFirstElement();
		//if (owner == thisPlayer)
			//showMessage("building");
	}

	maxSelection = minSelection + 3;
	if (maxSelection > (selectionList->getNumElements()-1))
	{
		maxSelection = selectionList->getNumElements() - 1;
		minSelection = maxSelection - 3;
		if (minSelection < 0)
			minSelection = 0;
	}

/*	showMessage("using builderclass");

	if (selectionList->getNumElements() >= 1)
	{
		data1.buildNum = minSelection;
		b1.setCallBack(handleClick, &data1);
		b1.setCallBackCancel(handleCancel, &data1);
		b1.setHelpText(((BuildItemClass*)selectionList->getElement(minSelection))->getDescription());

		if (selectionList->getNumElements() >= 2)
		{
			data2.buildNum = minSelection + 1;
			b2.setCallBack(handleClick, &data2);
			b2.setCallBackCancel(handleCancel, &data2);
			b2.setHelpText(((BuildItemClass*)selectionList->getElement(minSelection + 1))->getDescription());

			if (selectionList->getNumElements() >= 3)
			{
				data3.buildNum = minSelection + 2;
				b3.setCallBack(handleClick, &data3);
				b3.setCallBackCancel(handleCancel, &data3);
				b3.setHelpText(((BuildItemClass*)selectionList->getElement(minSelection + 2))->getDescription());

				if (selectionList->getNumElements() >= 4)
				{
					data4.buildNum = minSelection + 3;
					b4.setCallBack(handleClick, &data4);
					b4.setCallBackCancel(handleCancel, &data4);
					b4.setHelpText(((BuildItemClass*)selectionList->getElement(minSelection + 3))->getDescription());
				}
			}
		}
	}*/
	
	char temp[50];
	BuildItemClass* item;


	if (selectionList->getNumElements() >= 1)
	{
		data1.buildNum = minSelection;
		b1.setCallBack(handleClick, &data1);
		b1.setCallBackCancel(handleCancel, &data1);

		item = (BuildItemClass*)selectionList->getElement(minSelection);
		sprintf(temp, "%s: %d", item->getName(), round(priceModifier * item->getCost()));
		b1.setHelpText(temp);

		if (selectionList->getNumElements() >= 2)
		{
			data2.buildNum = minSelection + 1;
			b2.setCallBack(handleClick, &data2);
			b2.setCallBackCancel(handleCancel, &data2);
			
			item = (BuildItemClass*)selectionList->getElement(minSelection + 1);
			sprintf(temp, "%s: %d", item->getName(), round(priceModifier * item->getCost()));
			b2.setHelpText(temp);

			if (selectionList->getNumElements() >= 3)
			{
				data3.buildNum = minSelection + 2;
				b3.setCallBack(handleClick, &data3);
				b3.setCallBackCancel(handleCancel, &data3);
				
				item = (BuildItemClass*)selectionList->getElement(minSelection + 2);
				sprintf(temp, "%s: %d", item->getName(), round(priceModifier * item->getCost())); 
				b3.setHelpText(temp);

				if (selectionList->getNumElements() >= 4)
				{
					data4.buildNum = minSelection + 3;
					b4.setCallBack(handleClick, &data4);
					b4.setCallBackCancel(handleCancel, &data4);
					
					item = (BuildItemClass*)selectionList->getElement(minSelection + 3);
					sprintf(temp, "%s: %d", item->getName(), round(priceModifier * item->getCost())); 
					b4.setHelpText(temp);
				}
			}
		}
	}


}



void BuilderClass::checkSelectionList()

{

	BuildItemClass* tempBuildItem;
	selectionList->reset();
	while(selectionList->currentNotNull())
	{
		tempBuildItem = (BuildItemClass*)selectionList->removeCurrent();
		if (!buildList->findElement(tempBuildItem) && (tempBuildItem != currentItem))
			delete tempBuildItem;
	}

/*

	if(health < maxHealth)

	{

		repairButton.setVisible(true);

	}

	else

	{

		repairButton.setVisible(false);

	}*/

}



void BuilderClass::drawItems(int minX, int minY, int maxX, int maxY, int spacing)

{

	int count2 = 0;

	SDL_Rect	rectPos;



	rectPos.x = minX - 10;//+ 40;

	rectPos.y = minY - 10;//+ 190;

	rectPos.w = 10 + 64 + 10;

	rectPos.h = 10 + 48*4 + spacing*4;



	SDL_FillRect(screen, &rectPos, COLOUR_BLACK);



	for (int count = getMinBarSelection(); count <= getMaxBarSelection(); count++)

		((BuildItemClass*)selectionList->getElement(count))->draw(minX, minY + (48 + spacing)*count2++);

}



void BuilderClass::doDown()

{

	//fprintf(stdout, "maxSelection is %i, selectionList->getNumElements() is %i\n", maxSelection, selectionList->getNumElements());
	if ((selectionList->getNumElements() > 4) && (maxSelection < (selectionList->getNumElements()-1)))
	{
		minSelection++;
		maxSelection++;

		checkMinMaxSelection();
	}

}



void BuilderClass::doUp()

{

	if ((selectionList->getNumElements() > 4) && (minSelection > 0))
	{
		minSelection--;
		maxSelection--;

		checkMinMaxSelection();
	}

}



void BuilderClass::handleClick(void* data)
{
	BUILD_ITEM_DATA*	buildData = (BUILD_ITEM_DATA*)data;

	int					selection = buildData->buildNum;
	BuilderClass*		builder = (BuilderClass*)buildData->object;

	if (builder->isWaitingToPlace(selection))
		placingMode = true;
	else
		builder->netDoBuildCommand(shift, selection);
}

void BuilderClass::handleCancel(void* data)
{
	BUILD_ITEM_DATA*	buildData = (BUILD_ITEM_DATA*)data;

	int					selection = buildData->buildNum;
	BuilderClass*		builder = (BuilderClass*)buildData->object;

	builder->netDoCancelCommand(shift, selection);
}


void BuilderClass::insertItem(int newItemID)
{
	bool	foundInSelection = false,
			foundInBuild = false;
	BuildItemClass* tempBuildItem;
	
	selectionList->reset();
	while(selectionList->currentNotNull() && !foundInSelection)
	{
		tempBuildItem = reinterpret_cast<BuildItemClass*>(selectionList->getCurrent());
		if (tempBuildItem->getItemID() == newItemID)
			foundInSelection = true;

		selectionList->nextLink();
	}

	if (!foundInSelection)
	{
		buildList->reset();
		while(buildList->currentNotNull() && !foundInBuild)
		{
			tempBuildItem = reinterpret_cast<BuildItemClass*>(buildList->getCurrent());
			if (tempBuildItem->getItemID() == newItemID)
				foundInBuild = true;

			buildList->nextLink();
		}

		if (!foundInBuild)
		{
			if (currentItem && (currentItem->getItemID() == newItemID))
			{
				tempBuildItem = currentItem;
				foundInBuild = true;
			}
			else
				tempBuildItem = new BuildItemClass(newItemID);
		}

		selectionList->insertLast(tempBuildItem);

		if (selectionList->getNumElements() >= 1)
		{
			b1.setEnabled(true);
			b1.setVisible(true);

			if (selectionList->getNumElements() >= 2)
			{
				b2.setEnabled(true);
				b2.setVisible(true);

				if (selectionList->getNumElements() >= 3)
				{
					b3.setEnabled(true);
					b3.setVisible(true);

					if (selectionList->getNumElements() >= 4)
					{
						b4.setEnabled(true);
						b4.setVisible(true);
					}
				}
			}
		}
	}
}

void BuilderClass::buttonDown(void* data)
{
	((BuilderClass*)data)->doDown();
}

void BuilderClass::buttonUp(void* data)
{
	((BuilderClass*)data)->doUp();
}

void BuilderClass::buildRandom()
{
	netDoBuildCommand(false, getRandomInt(0, getNumSelections()-1));
}

bool BuilderClass::buildNum(bool multipleMode, int num)
{
	bool ok = false;
	BuildItemClass* item = (BuildItemClass*)selectionList->getElement(num);

	if (item != NULL)
	{
		ok = true;
		if (item->isOnHold())
			item->setOnHold(false);
		else
		{

			num = multipleMode ? 5 : 1;

			while (num--)

			{

				if (buildList->getNumElements() < MAXBUILDLIST)

				{
					item->incrementNumToBuild();
					buildList->insertLast(item);

					if (!currentItem)
						currentItem = (BuildItemClass*)buildList->getFirstElement();

				}

				else if (owner == thisPlayer)

				{

					showMessage("buildlist full");

					num = 0;

					ok = false;

				}

			}
		}
	}

	return ok;
}

void BuilderClass::buildType(int itemID)
{
	bool			found = false;
	int				pos = 0;
	BuildItemClass*	tempItem;

	selectionList->saveCurrent();
	selectionList->reset();
	while(selectionList->currentNotNull() && !found)
	{
		tempItem = (BuildItemClass*)selectionList->getCurrent();
		if (itemID == tempItem->getItemID())
			found = true;
		else
			pos++;

		selectionList->nextLink();
	}
	selectionList->restoreCurrent();

	if (found)
		netDoBuildCommand(false, pos);
}
void BuilderClass::cancelSelection(bool multipleMode, int num)
{

	int count = multipleMode ? 5 : 1;

	while (count--)

		cancelSelection((BuildItemClass*)selectionList->getElement(num));

}
void BuilderClass::cancelSelection(BuildItemClass* item)
{
	if  (item->getNumToBeBuilt() >= 1)
	{
		if (!item->isOnHold())

		{
			item->setOnHold(true);

			//if (owner == thisPlayer)

				//showMessage("on hold");

		}
		else
		{

			//if (owner == thisPlayer)

				//showMessage("cancelled");


			buildList->removeElement(item);
			item->decrementNumToBuild();
			if (item->getNumToBeBuilt() == 0)
			{
				if (item->isWaitingToPlace())
				{
					item->setWaitingToPlace(false);
					owner->returnCredits(item->getCost());
				}
				else
					owner->returnCredits(item->getProgress());

				item->setOnHold(false);
				item->setProgress(0);
			}

			if (buildList->getNumElements() == 0)
			{
				item->setOnHold(false);
				item->setProgress(0);
				currentItem = NULL;
			}
			else
			{
				currentItem = reinterpret_cast<BuildItemClass*>(buildList->getFirstElement());
				//if (owner == thisPlayer)
					//showMessage("building");
			}
		}
	}
}



void BuilderClass::loadBuildList(FILE* fp)
{
	bool	found;
	int		i,
			numItems,
			temp;
	double	tempDouble;
	BuildItemClass*	buildItem;

	selectionList->saveCurrent();
	fread(&numItems, sizeof(int), 1, fp);
	for (i = 0; i < numItems; i++)
	{
		fread(&temp, sizeof(int), 1, fp);
		
		found = false;
		selectionList->reset();
		while(selectionList->currentNotNull() && !found)
		{
			buildItem = (BuildItemClass*)selectionList->getCurrent();
			if (temp == buildItem->getItemID())
				found = true;

			selectionList->nextLink();
		}

		if (found)
		{
			buildList->insertLast(buildItem);
			buildItem->incrementNumToBuild();
			if (!currentItem)
				currentItem = buildItem;

			fread(&tempDouble, sizeof(double), 1, fp);
			buildItem->setCost(round(tempDouble));
			fread(&tempDouble, sizeof(double), 1, fp);
			buildItem->setProgress(round(tempDouble));
			fread(&temp, sizeof(int), 1, fp);
			buildItem->setOnHold(temp ? true : false);
			fread(&temp, sizeof(int), 1, fp);
			buildItem->setWaitingToPlace(temp ? true : false);
		}
		
	}
	selectionList->restoreCurrent();
}



void BuilderClass::setWaitingToPlace()
{
	if (currentItem != NULL)
	{
		if (owner == thisPlayer)
		{
			int count;
			for(count=0; count < selectionList->getNumElements(); count++)
			{
				if (currentItem == (BuildItemClass*)selectionList->getElement(count))
					break;
			}
			//showMessage("construction complete");
			switch (getOwner()->getHouse())
			{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					soundPlayer->playVOSound(SoundVO_AConst);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					soundPlayer->playVOSound(SoundVO_OConst);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					soundPlayer->playVOSound(SoundVO_HConst);
					break;
				default:
					break;
			}
		}

		if (currentItem->getItemID() >= Unit_Carryall)	 //if its a unit
		{

			buildList->removeFirstElement();
			currentItem->decrementNumToBuild();
			currentItem->setProgress(0);



			if (settings.playerType != CLIENT)

			{

				UnitClass* newUnit = (UnitClass*)getOwner()->createUnit(currentItem->getItemID());

				if (newUnit != NULL)

				{

					COORDTYPE spot = map->findDeploySpot(&location, &destination, &structureSize, newUnit);

					newUnit->deploy(&spot);

					if (getOwner()->isAI()

						&& (newUnit->getItemID() != Unit_Carryall)

						&& (newUnit->getItemID() != Unit_Harvester)

						&& (newUnit->getItemID() != Unit_MCV))

						newUnit->setAttackMode(AGGRESSIVE);

					if (owner == thisPlayer)

						showMessage("unit deployed");


					if (destination.x != NONE)
					{
						newUnit->setGuardPoint(&destination);
						newUnit->setDestination(&destination);
						newUnit->setAngle(round(8.0/256.0*dest_angle(newUnit->getLocation(), newUnit->getDestination())));
					}

					if (settings.playerType == SERVER)
					{
						netServerCreateObject(getOwner()->getPlayerNumber(), newUnit->getItemID(), spot.x, spot.y, newUnit->getAngle());
						if (destination.x != NONE)
							newUnit->netDoCommand(NULL, &destination, false);
					}
				}
			}
		}
		else		//its a structure

		{
			currentItem->setWaitingToPlace(true);
			if (owner == thisPlayer)

				showMessage("structure is ready to place");

		}

		if (!currentItem->isWaitingToPlace())
		{
			if (buildList->isEmpty())
				currentItem = NULL;
			else
			{
				currentItem = (BuildItemClass*)buildList->getFirstElement();
				//if (owner == thisPlayer)
				//	showMessage("building");
			}
		}
	}
}

void BuilderClass::unSetWaitingToPlace()
{
	if (currentItem)
	{
		currentItem->setWaitingToPlace(false);
		buildList->removeFirstElement();
		currentItem->decrementNumToBuild();
		currentItem->setProgress(0);

		if (buildList->isEmpty())
			currentItem = NULL;
		else
		{
			currentItem = (BuildItemClass*)buildList->getFirstElement();
			//if (owner == thisPlayer)
				//showMessage("building");
		}
	}
}

void BuilderClass::netDoBuildCommand(bool multipleMode, int num)
{
	if (settings.playerType == SINGLE)
		buildNum(multipleMode, num);
	else if (settings.playerType == SERVER)
	{
		buildNum(multipleMode, num);
		netServerBuildMessage(objectID, &location, multipleMode, num);
	}
	else
		netClientBuildMessage(objectID, &location, multipleMode, num);
}

void BuilderClass::netDoCancelCommand(bool multipleMode, int num)
{
	if (settings.playerType == SINGLE)
		cancelSelection(multipleMode, num);
	else if (settings.playerType == SERVER)
	{
		cancelSelection(multipleMode, num);
		netServerCancelMessage(objectID, &location, multipleMode, num);
	}
	else
		netClientCancelMessage(objectID, &location, multipleMode, num);
}

void BuilderClass::netSetWaitingToPlace()
{
	if (settings.playerType == SINGLE)
		setWaitingToPlace();
	else if (settings.playerType == SERVER)
	{
		netServerBuildComplete(objectID, &location);
		setWaitingToPlace();	//must be this order!! complete before waiting to place
	}
}

void BuilderClass::saveBuildList(FILE* fp)
{
	int		temp;
	double	tempDouble;
	BuildItemClass*	buildItem;

	temp = buildList->getNumElements();
	fwrite(&temp, sizeof(int), 1, fp);
	
	buildList->reset();
	while(buildList->currentNotNull())
	{
		buildItem = (BuildItemClass*)buildList->getCurrent();

		temp = buildItem->getItemID();
		fwrite(&temp, sizeof(int), 1, fp);
		tempDouble = buildItem->getCost();
		fwrite(&tempDouble, sizeof(double), 1, fp);
		tempDouble = buildItem->getProgress();
		fwrite(&tempDouble, sizeof(double), 1, fp);
		if (buildItem->isOnHold())
			temp = 1;
		else
			temp = 0;
		fwrite(&temp, sizeof(int), 1, fp);
		if (buildItem->isWaitingToPlace())
			temp = 1;
		else
			temp = 0;
		fwrite(&temp, sizeof(int), 1, fp);

		buildList->nextLink();
	}
}


bool BuilderClass::isOnHold(int num)
{
	return (reinterpret_cast<BuildItemClass*>(selectionList->getElement(num))->isOnHold());
}

bool BuilderClass::isTypeWaitingToPlace(int typeID)
{
	selectionList->reset();
	while(selectionList->currentNotNull())
	{
		if (((BuildItemClass*)selectionList->getCurrent())->isWaitingToPlace()
			&& (((BuildItemClass*)selectionList->getCurrent())->getItemID() == typeID))
			return true;
		selectionList->nextLink();
	}
	return false;
}


bool BuilderClass::isWaitingToPlace(int num)
{
	if (num == NONE)
		return (currentItem && currentItem->isWaitingToPlace());
	return (reinterpret_cast<BuildItemClass*>(selectionList->getElement(num)))->isWaitingToPlace();
}


int BuilderClass::getNumSelections()
{
	return selectionList->getNumElements();
}


int BuilderClass::getNumItemsToBuild()

{

	return buildList->getNumElements();

}



int BuilderClass::getNumItemsToBuild(int pictureNum)

{

	return (((BuildItemClass*)selectionList->getElement(pictureNum))->getNumToBeBuilt());

}


int BuilderClass::getMinBarSelection()
{
	return minSelection;
}


int BuilderClass::getMaxBarSelection()
{
	return maxSelection;
}


BuildItemClass* BuilderClass::getPlaceItem()
{
	return currentItem;
}

void BuilderClass::update()
{
	StructureClass::update();

	if(health == maxHealth)
	{
		//possible to upgrade
		
		if(upgradeLevels > 0 && curUpgradeLev < upgradeLevels && !upgrading && allowedToUpgrade &&
			settings.techLevel >= 4)
		{
			//repairButton.setText("Upgrade");
			repairButton.setSurface(UpgradeUp,NULL);
			repairButton.setPressedSurface(UpgradeDown, NULL);
			repairButton.setVisible(true);
			repairButton.setCallBack(upgradeCallback, this);
			repairButton.setHeight(UpgradeUp->h);
			repairButton.setWidth(UpgradeUp->w);
		}
	}
	else if(!upgrading)
	{
		/*RepairButton.setText("Repair");*/
		repairButton.setSurface(RepairUp, NULL);
		repairButton.setPressedSurface(RepairDown, NULL);
		repairButton.setHeight(RepairUp->h);
		repairButton.setWidth(RepairUp->w);
		repairButton.setCallBack(repairCallback, this);
	}

	if(upgrading)
	{
		upgradeProgress += 5;
		repairButton.setIsProgress(true);
		repairButton.setTotalProgress(upgradeCost);
		repairButton.setProgress(upgradeProgress);

		owner->takeCredits(upgradeProgress - lastUpgradeProgress);
		lastUpgradeProgress = upgradeProgress;

		if(upgradeProgress >= upgradeCost)
		{
			upgrading = false;
			curUpgradeLev++;
			this->checkSelectionList();

			upgradeProgress = 0;
			lastUpgradeProgress = 0;

			/*repairButton.setText("Repair");*/
			repairButton.setVisible(false);
			repairButton.setIsProgress(false);
			repairButton.setCallBack(repairCallback, this);
		}
	}
	
}

void BuilderClass::upgrade()
{
	if(owner->getAmountOfCredits() >= upgradeCost)
	{
		upgrading = true;
		showMessage("upgrading");
		upgradeProgress = 0;
		lastUpgradeProgress = 0;
	}
}

void BuilderClass::upgradeCallback(void *val)
{
	((BuilderClass *)val)->upgrade();
}

