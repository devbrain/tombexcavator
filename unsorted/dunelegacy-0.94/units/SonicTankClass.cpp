#include "../sand.h"
#include "../Items.h"

COORDTYPE sonicTankTurretOffset[] =
{
	{3, 1},
	{3, 1},
	{3, 1},
	{3, 1},
	{3, 1},
	{3, 1},
	{3, 1},
	{3, 1}
};

SonicTankClass::SonicTankClass(PlayerClass* newOwner) : TrackedUnit(newOwner)
{
	itemID = Unit_SonicTank;
	baseID = Unit_TankBase;

	armour = 7;
	speed = 0.5;
	maxHealth = 350;
	health = maxHealth;
	viewRange = SONICTANK_VIEWRANGE;
	radius = 6;
	weaponRange = SONICTANK_WEAPONRANGE;
	
	numWeapons = 1;
	bulletType[0] = Bullet_Sonic;
	primaryWeaponReloadTime = 150;

	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	turretGraphic = (SDL_Surface*)dataFile[itemID].dat;
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	numAttackSounds = 1;
	attackSound[0] = Sound_sonic;
}

SonicTankClass::~SonicTankClass()
{
}

void SonicTankClass::blitToScreen()
{
	SDL_Rect dest, source;
	dest.x = getDrawnX();
	dest.y = getDrawnY();
	dest.w = source.w = imageW;
	dest.h = source.h = imageH;
	source.y = 0;

	if (!destroyed)
	{
		source.x = drawnAngle*imageW;

		SDL_BlitSurface(graphic, &source, screen, &dest);	//blit base
		source.w = turretGraphic->w/NUM_ANGLES;
		source.h = turretGraphic->h;
		source.x = drawnAngle*source.w;
		source.y = 0;
		dest.x = getDrawnX() + sonicTankTurretOffset[drawnAngle].x;
		dest.y = getDrawnY() + sonicTankTurretOffset[drawnAngle].y;
		SDL_BlitSurface(turretGraphic, &source, screen, &dest);	//blit turret
		if (badlyDamaged)
			drawSmoke(round(realX - dborder->minX), round(realY - dborder->minY));
		if (selected)
			selectionBlitList->insertLast(this);
	}
	else
	{
		source.x = deathFrame*imageW;
		source.y = 0;
		SDL_BlitSurface(graphic, &source, screen, &dest);
	}
}

void SonicTankClass::handleDamage(int damage, ObjectClass *damager)
{
	if (!damager || (damager->getItemID() != Unit_SonicTank))
		ObjectClass::handleDamage(damage, damager);
}

bool SonicTankClass::canAttack(ObjectClass *object)
{
	return (ObjectClass::canAttack(object) && (object->getItemID() != Unit_SonicTank));
}

