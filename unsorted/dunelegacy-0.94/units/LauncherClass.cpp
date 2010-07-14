#include "../sand.h"
#include "../Items.h"

COORDTYPE launcherTurretOffset[] =
{
	{3, 0},
	{3, 1},
	{3, 1},
	{3, 1},
	{3, 0},
	{3, 1},
	{3, 1},
	{3, 1}
};

LauncherClass::LauncherClass(PlayerClass* newOwner) : TrackedUnit(newOwner)
{
	itemID = Unit_Launcher;
	baseID = Unit_TankBase;

	armour = 7;
	speed = 0.2;
	maxHealth = 300;
	health = maxHealth;
	radius = 6;
	turnSpeed /= 2;
	viewRange = LAUNCHER_VIEWRANGE;
	weaponRange = LAUNCHER_WEAPONRANGE;

	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	turretGraphic = (SDL_Surface*)dataFile[itemID].dat;
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	numWeapons = 2;
	bulletType[0] = Bullet_Rocket;
	primaryWeaponReloadTime = 285;

	numAttackSounds = 1;
	attackSound[0] = Sound_rocket;
}

LauncherClass::~LauncherClass()
{
}

void LauncherClass::blitToScreen()
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
		dest.x = getDrawnX() + launcherTurretOffset[drawnAngle].x;
		dest.y = getDrawnY() + launcherTurretOffset[drawnAngle].y;
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

bool LauncherClass::canAttack(ObjectClass* object)
{
	return (!object->wasDestroyed() && ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm) && object->isVisible(getOwner()->getTeam()));
}

