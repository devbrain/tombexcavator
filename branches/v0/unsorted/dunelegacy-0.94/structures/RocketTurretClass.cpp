#include "../sand.h"
#include "../Items.h"

RocketTurretClass::RocketTurretClass(PlayerClass* newOwner) : GunTurretClass(newOwner)
{
	itemID = Structure_RocketTurret;
	structureSize.x = 1;
	structureSize.y = 1;

	angle = getRandomInt(0, 7);
	drawnAngle = round(angle);
	attackMode = DEFENSIVE;

	attackSound = Sound_rocket;
	bulletType = Bullet_Rocket;
	
	maxHealth = ROCKETTURRET_HITPOINTS;
	powerRequirement = 15;
	health = maxHealth;
	weaponRange = ROCKETTURRET_WEAPONRANGE;
	viewRange = ROCKETTURRET_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_BuildingTurret].dat);
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

RocketTurretClass::~RocketTurretClass()
{
}

void RocketTurretClass::doSpecificStuff()
{
	if (getOwner()->hasPower() || (((settings.gameType == ORIGINAL) || (settings.gameType == SKIRMISH)) && getOwner()->isAI()))
		GunTurretClass::doSpecificStuff();
}

bool RocketTurretClass::canAttack(ObjectClass* object)
{
	if (!object->wasDestroyed() && ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm) && object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}

