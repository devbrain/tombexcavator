#include "../sand.h"
#include "../Items.h"

QuadClass::QuadClass(PlayerClass* newOwner) : GroundUnit(newOwner)
{
	baseID = itemID = Unit_Quad;

	speed = 0.64;

	armour = 6;
	maxHealth = 350;
	health = maxHealth;
	radius = 6;
	viewRange = QUAD_VIEWRANGE;
	weaponRange = QUAD_WEAPONRANGE;
		
	numWeapons = 2;
	bulletType[0] = Bullet_Gun;
	primaryWeaponReloadTime = 100;

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
QuadClass::~QuadClass()
{
}

