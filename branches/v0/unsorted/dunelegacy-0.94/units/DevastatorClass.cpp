#include "../sand.h"
#include "../Items.h"

COORDTYPE	devastatorTurretOffset[] =
{
	{2, -4},
	{-1, -3},
	{0, -4},
	{1, -3},
	{-2, -4},
	{0, -3},
	{-1, -3},
	{0, -3}
};

DevastatorClass::DevastatorClass(PlayerClass* newOwner) : TrackedUnit(newOwner)
{
	itemID = Unit_Devastator;
	baseID = Unit_DevastatorBase;

	deathFrame = Object_LargeUnitExplosion;
	numDeathFrames = 5;

	armour = 13;
	speed = 0.25;
	maxHealth = 800;
	health = maxHealth;
	radius = 7;
	viewRange = DEVASTATOR_VIEWRANGE;
	weaponRange = DEVASTATOR_WEAPONRANGE;
	devastateTimer = 0;
	devastated = false;

	numWeapons = 2;
	bulletType[0] = Bullet_Shell;
	primaryWeaponReloadTime = 150;

	graphic = (SDL_Surface*)dataFile[baseID].dat;
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	turretGraphic = (SDL_Surface*)dataFile[itemID].dat;
	turretGraphic = copySurface(turretGraphic);
	mapImageHouseColour(turretGraphic, getOwner()->getColour());
	
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	numAttackSounds = 1;
	attackSound[0] = Sound_smallExplosion;
	destroySound[0] = Sound_largeExplosion;
}
DevastatorClass::~DevastatorClass()
{
	SDL_FreeSurface(turretGraphic);
}

void DevastatorClass::blitToScreen()
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
		dest.x = getDrawnX() + devastatorTurretOffset[drawnAngle].x;
		dest.y = getDrawnY() + devastatorTurretOffset[drawnAngle].y;
		SDL_BlitSurface(turretGraphic, &source, screen, &dest);	//blit turret
		if (badlyDamaged)
			drawSmoke(round(realX - dborder->minX), round(realY - dborder->minY));
		if (selected)
			selectionBlitList->insertLast(this);
	}
	else if (devastated)
	{
		for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
		{
			dest.w = source.w = imageW;
			dest.h = source.h = imageH;
			source.x = source.w * deathFrame;
			dest.x = getDrawnX() + (i - 1)*BLOCKSIZE;
			dest.y = getDrawnY() + (j - 1)*BLOCKSIZE;
			SDL_BlitSurface(explosion[i][j], &source, screen, &dest);
		}
	}
	else
	{
		source.x = deathFrame*imageW;
		source.y = 0;
		SDL_BlitSurface(graphic, &source, screen, &dest);
	}
}

void DevastatorClass::devastate()
{
	if (!destroyed)
	{
		int i, j, num;
		COORDTYPE realPos;

		destroy();
		devastated = true;
		numDeathFrames = 5;
		for(i = 0; i < 3; i++)
		for(j = 0; j < 3; j++)
		{
			num = getRandomInt(0, 1) ? Object_LargeExplosion1 : Object_LargeExplosion2;	//50% chance of either
			explosion[i][j] = (SDL_Surface*)dataFile[num].dat;

			realPos.x = (short)realX + (i - 1)*BLOCKSIZE;
			realPos.y = (short)realY + (j - 1)*BLOCKSIZE;
			map->damage(this, owner, &realPos, itemID, 150, 0, 16, false);
			if (numDestroySounds)
				soundPlayer->playSfxSound(destroySound[0]);
		}

		imageW = explosion[0][1]->w/numDeathFrames;
		imageH = explosion[0][1]->h;
		xOffset = (imageW - BLOCKSIZE)/2;		    //this is where it actually draws the graphic
		yOffset = (imageH - BLOCKSIZE)/2;		    //cause it draws at top left, not middle
	}
}

void DevastatorClass::startDevastate()
{
	if (devastateTimer <= 0)
		devastateTimer = 200;
}

void DevastatorClass::netDevastate()

{

	if (settings.playerType != CLIENT)

	{

		startDevastate();

		if (settings.playerType == SERVER)

			netServerDevastate(getObjectID(), getX(), getY());

	}

	else

		netClientDevastate(getObjectID(), getX(), getY());

}

void DevastatorClass::update()
{
	if (active)
	{
		UnitClass::update();

		if ((devastateTimer > 0) && (--devastateTimer == 0))
			devastate();
	}
}

