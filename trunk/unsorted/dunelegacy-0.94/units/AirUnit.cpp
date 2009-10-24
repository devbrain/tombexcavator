#include "../sand.h"
#include "../Items.h"

AirUnit::AirUnit(PlayerClass* newOwner) : UnitClass(newOwner)
{
	aFlyingUnit = true;

	attackMode = DEFENSIVE;

	deathFrame = Object_AirUnitExplosion;
	numDeathFrames = 5;
}

AirUnit::~AirUnit()
{
}

void AirUnit::assignToMap(COORDTYPE* pos)
{
	if (map->cellExists(pos->x, pos->y))
	{
		map->cell[pos->x][pos->y].assignAirUnit(this);
		map->viewMap(owner->getTeam(), &location, viewRange);
	}
}

void AirUnit::checkPos()
{
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

bool AirUnit::canPass(int xPos, int yPos)
{
	return (map->cellExists(xPos, yPos) && (!map->getCell(xPos, yPos)->hasAnAirUnit()));
}

