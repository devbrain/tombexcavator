#include "../sand.h"
#include "../Items.h"

Ornithopter::Ornithopter(PlayerClass* newOwner) : AirUnit(newOwner)
{
	baseID = itemID = Unit_Ornithopter;

	armour = 3;
	speed = 1.6;
	maxHealth = 250;
	health = maxHealth;
	radius = 6;
	viewRange = ORNITHOPTER_VIEWRANGE;
	weaponRange = ORNITHOPTER_WEAPONRANGE;

	nextSpotAngle = drawnAngle;
	turnSpeed = 0.125;
	weaponRange = 5;
	viewRange = DEFAULT_UNITVIEWRANGE;

	attackMode = DEFENSIVE;

	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h/3;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	numWeapons = 1;
	bulletType[0] = Bullet_SmallRocket;
	primaryWeaponReloadTime = 300;

	numAttackSounds = 1;
	attackSound[0] = Sound_smallRocket;
}
Ornithopter::~Ornithopter()
{
}

void Ornithopter::checkPos()
{
	AirUnit::checkPos();
	if (++drawnFrame >= 3)
		drawnFrame = 0;
}

bool Ornithopter::canAttack(ObjectClass* object)
{
	if (!object->wasDestroyed() && ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm) && object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}

