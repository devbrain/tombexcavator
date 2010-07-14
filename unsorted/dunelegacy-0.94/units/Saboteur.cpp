#include "../sand.h"
#include "../Items.h"

Saboteur::Saboteur(PlayerClass* newOwner) : InfantryClass(newOwner)
{
	baseID = itemID = Unit_Saboteur;

	infantry = true;

	armour = 1;
	maxHealth = 100;
	health = maxHealth;
	radius = 2;
	speed = 0.3;
	viewRange = TROOPER_VIEWRANGE;
	weaponRange = 0;
	
	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/4;
	imageH = graphic->h/3;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	deathFrame = Object_DeadInfantry;
	numDeathFrames = 2;

	numWeapons = 0;

	setVisible(VIS_ALL, false);
	setVisible(getOwner()->getTeam(), true);

	setAttackMode(SCOUT);
}
Saboteur::~Saboteur()
{
}

void Saboteur::checkPos()
{
	bool	canBeSeen[MAX_PLAYERS+1];
	int		i, j;

	InfantryClass::checkPos();
	if (active)
	{
		for(i = 1; i <= MAX_PLAYERS; i++)
			canBeSeen[i] = false;

		for(i = location.x - 2; (i <= location.x + 2); i++)
		for(j = location.y - 2; (j <= location.y + 2); j++)
			if (map->cellExists(i, j) && map->getCell(i, j)->hasAnObject())
				canBeSeen[map->getCell(i, j)->getObject()->getOwner()->getTeam()] = true;

		for(i = 1; i <= MAX_PLAYERS; i++)
			setVisible(i, canBeSeen[i]);
		setVisible(getOwner()->getTeam(), true);	//owner team can always see it
		//setVisible(thisPlayer->getTeam(), true);

		if (justStoppedMoving)
		{	//check to osee if close enough to blow up target
			if (target && target->isAStructure() && (getOwner()->getTeam() != target->getOwner()->getTeam()))
			{
				COORDTYPE	closestPoint;

				closestPoint = target->getClosestPoint(&location);
		        
				if (blockDistance(&location, &closestPoint) <= 1.5)	
				{
					target->netDestroy();
					netDestroy();
				}
			}
		}
	}
}

void Saboteur::deploy(COORDTYPE* newLocation)
{
	UnitClass::deploy(newLocation);

	setVisible(VIS_ALL, false);
	setVisible(getOwner()->getTeam(), true);
}

bool Saboteur::canAttack(ObjectClass* object)
{
	if (!object->wasDestroyed() && object->isAStructure() && (object->getOwner()->getTeam() != owner->getTeam()) && object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}

