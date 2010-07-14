#include "../sand.h"
#include "../Items.h"

TrackedUnit::TrackedUnit(PlayerClass* newOwner) : GroundUnit(newOwner)
{
	tracked = true;
}

TrackedUnit::~TrackedUnit()
{
}


void TrackedUnit::checkPos()
{
	GroundUnit::checkPos();
	if (active && justStoppedMoving)
		map->getCell(location.x, location.y)->squash();
}

bool TrackedUnit::canPass(int xPos, int yPos)
{
	bool passable = false;
	if (map->cellExists(xPos, yPos))
	{
		TerrainClass* cell = map->getCell(xPos, yPos);
		
		if (!cell->isMountain())
		{
			if (!cell->hasAGroundObject())
				passable = true;
			else if (!cell->hasANonInfantryGroundObject() && (cell->getInfantryTeam() != getOwner()->getTeam()))
				passable = true;
		}
	}

	return passable;
}

