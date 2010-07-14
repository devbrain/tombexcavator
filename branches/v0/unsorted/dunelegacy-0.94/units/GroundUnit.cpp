#include "../sand.h"
#include "../Items.h"

GroundUnit::GroundUnit(PlayerClass* newOwner) : UnitClass(newOwner)
{
	aGroundUnit = true;

	awaitingPickup = false;

	noCloserPointCount = 0;
}

GroundUnit::~GroundUnit()
{
}

void GroundUnit::checkPos()
{
	if (justStoppedMoving)
	{
		realX = location.x*BLOCKSIZE + BLOCKSIZE/2;
		realY = location.y*BLOCKSIZE + BLOCKSIZE/2;
		findTargetTimer = 0;	//allow a scan for new targets now

		//if (settings.playerType == SERVER)
		//	netDoConsistancy();

		if (map->getCell(&location)->isBloom())
		{
			COORDTYPE realPos;

			realPos.x = (short)realX;
			realPos.y = (short)realY;
			map->getCell(&location)->damageCell(this, getOwner(), &realPos, NONE, 500, NONE, NONE, false);
		}
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

void GroundUnit::requestCarryall()
{
	if (getOwner()->hasCarryalls())
	{
		Carryall* carryall = NULL;
		UnitClass* unit;

		unitList->saveCurrent();
		unitList->reset();
		while((carryall == NULL) && unitList->currentNotNull())
		{
			unit = (UnitClass*)unitList->getCurrent();
			if ((unit->getOwner() == owner) && (unit->getItemID() == Unit_Carryall))
			{
				if (((Carryall*)unit)->isRespondable() && !((Carryall*)unit)->isBooked())
				{
					carryall = (Carryall*)unit;
					carryall->setTarget(this);
					carryall->clearPath();
					bookedCarrier = carryall;
					//setDestination(&location);	//stop moving, and wait for carryall to arrive
				}
			}
			unitList->nextLink();
		}
		unitList->restoreCurrent();
	}
}

void GroundUnit::setPickedUp(UnitClass* newCarrier)
{
	UnitClass::setPickedUp(newCarrier);
	awaitingPickup = false;
	bookedCarrier = NULL;
	pathList.clearList();
}

void GroundUnit::bookCarrier(UnitClass* newCarrier)
{
	bookedCarrier = newCarrier;
	awaitingPickup = (bookedCarrier != NULL);
}

void GroundUnit::navigate()
{
	if (!awaitingPickup)
		UnitClass::navigate();
}

void GroundUnit::handleDamage(int damage, ObjectClass* damager)
{

	ObjectClass::handleDamage(damage, damager);

	//better make sure infantry doesnt get picked up
	if(badlyDamaged && !isInfantry())
	{
		requestCarryall();
	}
}

