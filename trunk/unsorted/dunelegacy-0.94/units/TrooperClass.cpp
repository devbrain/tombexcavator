#include "../sand.h"
#include "../Items.h"

TrooperClass::TrooperClass(PlayerClass* newOwner) : InfantryClass(newOwner)
{
	baseID = itemID = Unit_Trooper;

	infantry = true;

	armour = 1;
	maxHealth = 100;
	health = maxHealth;
	radius = 2;
	speed = 0.2;
	viewRange = TROOPER_VIEWRANGE;
	weaponRange = TROOPER_WEAPONRANGE;
	
	numWeapons = 1;
	bulletType[0] = Bullet_SmallRocket;
	primaryWeaponReloadTime = 300;

	SDL_FreeSurface(graphic);
	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/4;
	imageH = graphic->h/3;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;
	

	deathFrame = Object_DeadInfantry;
	numDeathFrames = 2;

	numAttackSounds = 1;
	attackSound[0] = Sound_smallRocket;
}
TrooperClass::~TrooperClass()
{
}

bool TrooperClass::canAttack(ObjectClass* object)
{
	if (!object->wasDestroyed() && ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm) && object->isVisible(getOwner()->getTeam()))
	{
		if(object->isAStructure() && object->getItemID() != Structure_Wall && object->getHealthColour() == COLOUR_RED)
			weaponRange = 0;
		else
			weaponRange = TROOPER_WEAPONRANGE;

		return true;
	}
	else
		return false;
}

