#include "../sand.h"
#include "../Items.h"

COORDTYPE	siegeTurretOffset[] =
{
	{2, -4},
	{1, -5},
	{0, -5},
	{-1, -5},
	{-2, -4},
	{-1, -2},
	{-1, -4},
	{1, -2}
};

SiegeTankClass::SiegeTankClass(PlayerClass* newOwner) : TankClass(newOwner)
{
	itemID = Unit_SiegeTank;
	baseID = Unit_SiegeTankBase;

	armour = 12;
	speed = 0.3;
	maxHealth = 600;
	health = maxHealth;
	radius = 7;
	viewRange = SIEGETANK_VIEWRANGE;
	weaponRange = SIEGETANK_WEAPONRANGE;
	
	numWeapons = 2;
	bulletType[0] = Bullet_Shell;
	primaryWeaponReloadTime = 150;

	SDL_FreeSurface(graphic);
	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	turretGraphic = (SDL_Surface*)(dataFile[itemID].dat);
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	deathFrame = Object_LargeUnitExplosion;
	numDeathFrames = 5;

	numAttackSounds = 1;
	attackSound[0] = Sound_smallExplosion;
	destroySound[0] = Sound_largeExplosion;
}
SiegeTankClass::~SiegeTankClass()
{
}

void SiegeTankClass::blitToScreen()
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
		source.x = drawnTurretAngle*source.w;
		source.y = 0;
		dest.x = getDrawnX() + siegeTurretOffset[drawnTurretAngle].x;
		dest.y = getDrawnY() + siegeTurretOffset[drawnTurretAngle].y;
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

