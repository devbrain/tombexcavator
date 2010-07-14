#include "../sand.h"
#include "../Items.h"

#define STARPORT_ARRIVETIME			5000
#define STARPORT_CHANGE_PRICETIME	5000
#define STARPORT_MIN_MOD			65
#define STARPORT_MAX_MOD			125

extern MessageBox messageBox;
StarPortClass::StarPortClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_StarPort;

	structureSize.x = 3;
	structureSize.y = 3;

	builder = true;

	maxHealth = 3000;
	powerRequirement = 80;
	health = maxHealth;
	viewRange = STARPORT_VIEWRANGE;

	priceModifier = 1.0;
	modifierTimer = STARPORT_CHANGE_PRICETIME;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building33].dat);
	imageW = graphic->w/3;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	animFrames = 3;
	animFrame = 0;
	isAnimating = 0;

	arrivalTimer = 0;
	checkSelectionList();

	orderSurf = copySurface((SDL_Surface *)dataFile[UI_Order].dat);
	orderSurfPressed = copySurface((SDL_Surface *)dataFile[UI_Order_Pressed].dat);

	cancelSurf = copySurface((SDL_Surface *)dataFile[UI_Cancel].dat);
	cancelSurfPressed = copySurface((SDL_Surface *)dataFile[UI_Cancel_Pressed].dat);

	mapImageHouseColour(orderSurf, getOwner()->getColour());
	mapImageHouseColour(orderSurfPressed, getOwner()->getColour());
	mapImageHouseColour(cancelSurf, getOwner()->getColour());
	mapImageHouseColour(cancelSurfPressed, getOwner()->getColour());

	//order.setText("Order");
	order.setSurface(orderSurf, NULL);
	order.setPressedSurface(orderSurfPressed, NULL);
	order.setCallBack(netPlaceOrder, this);
	order.setX(gameBarPos.x + BUILDERBUTTONSMINX - 17);
	order.setY(BUILDERBUTTONSMAXY + 10 + 26);
	order.setWidth(48);
	order.setHeight(26);
	interfaceContainer->addChild(&order);
	order.setVisible(true);

//	cancel.setText("Cancel");
	cancel.setSurface(cancelSurf, NULL);
	cancel.setPressedSurface(cancelSurfPressed, NULL);
	cancel.setCallBack(netCancelOrder, this);
	cancel.setX(gameBarPos.x + BUILDERBUTTONSMINX - 17 + 48);
	cancel.setY(BUILDERBUTTONSMAXY + 10 + 26);
	cancel.setWidth(48);
	cancel.setHeight(26);
	interfaceContainer->addChild(&cancel);
	cancel.setVisible(true);


}
StarPortClass::~StarPortClass()
{
}

bool StarPortClass::buildNum(bool multipleMode, int num)
{
	bool ok = false;
	BuildItemClass* item = (BuildItemClass*)selectionList->getElement(num);

	if (item != NULL)
	{
		ok = true;
		num = multipleMode ? 5 : 1;
		while (num--)
		{
			if ((getOwner()->getAmountOfCredits() >= item->getCost()) && (buildList->getNumElements() < MAXBUILDLIST))
			{
				item->incrementNumToBuild();
				buildList->insertLast(item);
				//owner->takeCredits((double)item->getCost());
				
				owner->takeCredits((double)round(priceModifier * item->getCost()));
				

			}
			else
			{
				soundPlayer->playSfxSound(Sound_cannot);
				ok = false;
			}
		}
	}
	return ok;
}

void StarPortClass::buildRandom()
{
	BuildItemClass* item;

	do
		item = reinterpret_cast<BuildItemClass*>(selectionList->getRandomElement());
	while ((item->getItemID() == Unit_Harvester) || (item->getItemID() == Unit_MCV) || (item->getItemID() == Unit_Carryall));
	buildType(item->getItemID());
}

void StarPortClass::cancelSelection(BuildItemClass* item)
{
	if  (item->getNumToBeBuilt() >= 1)
	{
		//if (owner == thisPlayer)
		//	showMessage("cancelled");

		item->decrementNumToBuild();
		buildList->removeElement(item);
		
		//owner->addCredits(item->getCost());
		owner->addCredits((double)round(priceModifier * item->getCost()));
				
	}
}

void StarPortClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	insertItem(Unit_SiegeTank);
	insertItem(Unit_Launcher);
	insertItem(Unit_Tank);
	insertItem(Unit_Quad);

	if(owner->getHouse() == HOUSE_ORDOS)
		insertItem(Unit_Raider);
	else
		insertItem(Unit_Trike);

	insertItem(Unit_Carryall);

	if(owner->getHouse() != HOUSE_HARKONNEN || owner->getHouse() != HOUSE_SARDAUKAR)
		insertItem(Unit_Ornithopter);

	insertItem(Unit_Harvester);
	insertItem(Unit_MCV);

	if (arrivalTimer != 0)
	{	
		BuildItemClass*	item;

		b1.setEnabled(false);
		b2.setEnabled(false);
		b3.setEnabled(false);
		b4.setEnabled(false);

		selectionList->reset();
		while (selectionList->currentNotNull())
		{
			item = (BuildItemClass*)selectionList->getCurrent();
			item->setProgress((int)item->getCost());

			selectionList->nextLink();
		}
	}

	checkMinMaxSelection();
}

void StarPortClass::doSpecificStuff()
{
	if(modifierTimer > 0)
	{
		if(--modifierTimer == 0)
		{
			char temp[50];

			priceModifier = (double)getRandomInt(STARPORT_MIN_MOD, STARPORT_MAX_MOD)/100;
			
			checkSelectionList();
			sprintf(temp, "Starport Modifier Now %.2f", priceModifier);
			showMessage(temp);

			modifierTimer = STARPORT_CHANGE_PRICETIME;
		}
	}

	if (arrivalTimer > 0)
	{
		int region = arrivalTimer/(STARPORT_ARRIVETIME/10);
		if (--arrivalTimer == 0)
		{
			 if (settings.playerType != CLIENT)
			 {
				BuildItemClass*	item;
				Frigate*		frigate;
				UnitClass*		unit;
				COORDTYPE		pos;

				//make a frigate with all the cargo
				frigate = (Frigate*)owner->createUnit(Unit_Frigate);
				pos = map->findClosestEdgePoint(getLocation(), getStructureSize());
				frigate->deploy(&pos);
				frigate->setTarget(this);
				frigate->setDestination(&getClosestPoint(frigate->getLocation()));

				if (pos.x == 0)
					frigate->setAngle(RIGHT);
				else if (pos.x == map->sizeX-1)
					frigate->setAngle(LEFT);
				else if (pos.y == 0)
					frigate->setAngle(DOWN);
				else if (pos.y == map->sizeY-1)
					frigate->setAngle(UP);

				if (settings.playerType == SERVER)
				{
					netServerCreateObject(getOwner()->getPlayerNumber(), Unit_Frigate, pos.x, pos.y, frigate->getAngle());
					netServerObjectMessage(frigate->getObjectID(), getObjectID(), pos.x, pos.y, getX(), getY(), true);
					netServerCancelOrder(getOwner()->getPlayerNumber(), getObjectID(), getX(), getY());	//tell clients to reset order form thing
				}

				buildList->reset();
				while (buildList->currentNotNull())
				{
					item = (BuildItemClass*)buildList->getCurrent();
					unit = (UnitClass*)owner->createUnit(item->getItemID());
					if (unit)
					{
						unit->setPickedUp(frigate);
						frigate->giveCargo(unit);
						item->decrementNumToBuild();
						buildList->removeCurrent();

						if (settings.playerType == SERVER)
							netServerCreateObject(getOwner()->getPlayerNumber(), unit->getItemID(), NONE, NONE, NONE);
					}
				}

				//reset the starport
				selectionList->reset();
				while (selectionList->currentNotNull())
				{
					item = (BuildItemClass*)selectionList->getCurrent();
					item->setNumToBuild(0);
					item->setProgress(0);

					selectionList->nextLink();
				}

				b1.setEnabled(true);
				b2.setEnabled(true);
				b3.setEnabled(true);
				b4.setEnabled(true);
				order.setEnabled(true);
				cancel.setEnabled(true);
			 }
			 else
				 arrivalTimer = NONE;

			if (getOwner() == thisPlayer)
			{
				showMessage("Frigate has arrived");
				switch (getOwner()->getHouse())
				{
				case(HOUSE_ATREIDES): case(HOUSE_FREMEN):
					soundPlayer->playVOSoundDual(SoundVO_Afrigate, SoundVO_Aarrive);
					break;
				case(HOUSE_ORDOS): case(HOUSE_MERCENARY):
					soundPlayer->playVOSoundDual(SoundVO_Ofrigate, SoundVO_Oarrive);
					break;
				case(HOUSE_HARKONNEN): case(HOUSE_SARDAUKAR):
					soundPlayer->playVOSoundDual(SoundVO_Hfrigate, SoundVO_Harrive);
					break;
				default:
					break;
				}
			}

			//Stop it from animating
			isAnimating = 0;
			animFrame = 0;
		}
		else
		{
			if ((getOwner() == thisPlayer) && (arrivalTimer/(STARPORT_ARRIVETIME/10) != region))
			{
				char temp[50];

				sprintf(temp, "Frigate arrival in t - %d", region);
				showMessage(temp);
				
				messageBox.addUrgentMessage(temp);
			}
		}
	}
}

void StarPortClass::setArrivalTimer(int newArrivalTimer)
{
	if (newArrivalTimer >= NONE)
	{
		arrivalTimer = newArrivalTimer;
		b1.setEnabled(arrivalTimer == 0);
		b2.setEnabled(arrivalTimer == 0);
		b3.setEnabled(arrivalTimer == 0);
		b4.setEnabled(arrivalTimer == 0);
		order.setEnabled(arrivalTimer == 0);
		cancel.setEnabled(arrivalTimer == 0);
	}
}

void StarPortClass::placeOrder()
{
	BuildItemClass*	item;

	if (buildList->getNumElements() > 0)
	{
		selectionList->reset();
		while (selectionList->currentNotNull())
		{
			item = (BuildItemClass*)selectionList->getCurrent();
			item->setProgress((int)item->getCost());

			selectionList->nextLink();
		}

		arrivalTimer = STARPORT_ARRIVETIME;
		b1.setEnabled(false);
		b2.setEnabled(false);
		b3.setEnabled(false);
		b4.setEnabled(false);
		order.setEnabled(false);
		cancel.setEnabled(false);

		isAnimating = 1;
	}
}

void StarPortClass::netPlaceOrder(void* object)
{
	StarPortClass*	port = (StarPortClass*)object;

	port->placeOrder();		
	if (settings.playerType == SERVER)
		netServerPlaceOrder(port->getOwner()->getPlayerNumber(), port->getObjectID(), port->getX(), port->getY());
	else if (settings.playerType == CLIENT)
		netClientPlaceOrder(port->getOwner()->getPlayerNumber(), port->getObjectID(), port->getX(), port->getY());
}

void StarPortClass::cancelOrder()
{
	if ((arrivalTimer == 0) || (settings.playerType == CLIENT))
	{
		BuildItemClass*	item;

		selectionList->reset();
		while (selectionList->currentNotNull())
		{
			item = (BuildItemClass*)selectionList->getCurrent();
			if (arrivalTimer == 0)	//only if not client refresh cancel
			{
				//getOwner()->returnCredits(item->getCost()*item->getNumToBeBuilt());
				//owner->takeCredits((double)round(priceModifier * item->getCost()));
				getOwner()->addCredits(round(priceModifier * item->getCost() * item->getNumToBeBuilt()));
			}
			item->setNumToBuild(0);
			item->setProgress(0);

			selectionList->nextLink();
		}
		buildList->clearList();

		b1.setEnabled(true);
		b2.setEnabled(true);
		b3.setEnabled(true);
		b4.setEnabled(true);
		order.setEnabled(true);
		cancel.setEnabled(true);
		arrivalTimer = 0;
	}
}

void StarPortClass::netCancelOrder(void* object)
{
	StarPortClass*	port = (StarPortClass*)object;

	if (settings.playerType != CLIENT)
	{
		port->cancelOrder();		
		if (settings.playerType == SERVER)
			netServerCancelOrder(port->getOwner()->getPlayerNumber(), port->getObjectID(), port->getX(), port->getY());
	}
	else
		netClientCancelOrder(port->getOwner()->getPlayerNumber(), port->getObjectID(), port->getX(), port->getY());
}

/*
void StarPortClass::checkMinMaxSelection()
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


	char temp[50];
	BuildItemClass* item;

	showMessage("in checkMinMax()");

	if (selectionList->getNumElements() >= 1)
	{
		data1.buildNum = minSelection;
		b1.setCallBack(handleClick, &data1);
		b1.setCallBackCancel(handleCancel, &data1);

		item = (BuildItemClass*)selectionList->getElement(minSelection);
		sprintf(temp, "%s- %d", item->getName(), round(priceModifier * item->getCost()));
		b1.setHelpText(temp);

		if (selectionList->getNumElements() >= 2)
		{
			data2.buildNum = minSelection + 1;
			b2.setCallBack(handleClick, &data2);
			b2.setCallBackCancel(handleCancel, &data2);
			
			item = (BuildItemClass*)selectionList->getElement(minSelection + 1);
			sprintf(temp, "%s- %d", item->getName(), round(priceModifier * item->getCost()));
			b1.setHelpText(temp);

			if (selectionList->getNumElements() >= 3)
			{
				data3.buildNum = minSelection + 2;
				b3.setCallBack(handleClick, &data3);
				b3.setCallBackCancel(handleCancel, &data3);
				
				item = (BuildItemClass*)selectionList->getElement(minSelection + 2);
				sprintf(temp, "%s- %d", item->getName(), round(priceModifier * item->getCost())); 
				b3.setHelpText(temp);

				if (selectionList->getNumElements() >= 4)
				{
					data4.buildNum = minSelection + 3;
					b4.setCallBack(handleClick, &data4);
					b4.setCallBackCancel(handleCancel, &data4);
					
					item = (BuildItemClass*)selectionList->getElement(minSelection + 3);
					sprintf(temp, "%s- %d", item->getName(), round(priceModifier * item->getCost())); 
					b3.setHelpText(temp);
				}
			}
		}
	}
}
*/

