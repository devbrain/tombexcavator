#include "../sand.h"
#include "../Items.h"

TrikeClass::TrikeClass(PlayerClass* newOwner) : GroundUnit(newOwner)
{
	baseID = itemID = Unit_Trike;

	speed = 1.0;

	armour = 4;
	maxHealth = 250;
	health = maxHealth;
	radius = 4;
	viewRange = TRIKE_VIEWRANGE;
	weaponRange = TRIKE_WEAPONRANGE;
	
	numWeapons = 2;
	bulletType[0] = Bullet_Gun;
	primaryWeaponReloadTime = 160;

	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	numAttackSounds = 1;
	attackSound[0] = Sound_machineGun;
}
TrikeClass::~TrikeClass()
{
}

