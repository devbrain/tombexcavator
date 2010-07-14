#include "sand.h"

/*
 *    Ships class thingy, by Anthony Cole
 *
 */

BulletClass::BulletClass(ObjectClass* newShooter, COORDTYPE* newRealLocation, COORDTYPE* newRealDestination, int newItemID, bool air)
{
	groundBlocked = false;
	airAttack = air;
	int inaccuracy;

	itemID = newItemID;

	destroyed = false;

	shooter = newShooter;
	if (shooter)
		owner = shooter->getOwner();
	else
		owner = NULL;

	deathFrame = itemID;
	deathSound = NONE;
	numDeathFrames = 1;
	numFrames = 1;
	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	imageH = graphic->h;

	switch(itemID)
	{
	case(Bullet_Inf_Gun):
		damagePiercing = 2;
		damageRadius = 2;
		groundBlocked = true;
		inaccuracy = 2;
		damage = 5;
		speed = 9.0;
		break;
	case(Bullet_Gun):
		damagePiercing = 2;
		damageRadius = 2;
		groundBlocked = true;
		inaccuracy = 2;
		damage = 18;
		speed = 9.0;
		break;
	case(Bullet_DRocket):
		damagePiercing = 0;
		damageRadius = 16;
		inaccuracy = 16;
		deathFrame = Bullet_GasHit;
		deathSound = Sound_gas;
		numFrames = 16;
		numDeathFrames = 5;
		damage = 0;
		speed = 4.0;
		break;
	case(Bullet_LargeRocket):
		damagePiercing = 8;
		damageRadius = 16;
		inaccuracy = 0;
		deathFrame = Bullet_RocketHit;
		deathSound = Sound_largeExplosion;
		numFrames = 16;
		numDeathFrames = 5;
		damage = 100;
		speed = 4.0;
		break;
	case(Bullet_Rocket):
		damagePiercing = 8;
		damageRadius = 4;
		if (newShooter->getItemID() == Unit_Launcher)
			inaccuracy = 20;
		else
			inaccuracy = 8;
		deathFrame = Bullet_RocketHit;
		numDeathFrames = 5;
		numFrames = 16;
		damage = 60;//80
		speed = 5.0;
		break;
	case(Bullet_Shell):
		damagePiercing = 12;
		damageRadius = 3;
		groundBlocked = true;
		inaccuracy = 6;
		deathFrame = Bullet_ShellHit;
		numDeathFrames = 3;
		damage = 30;//50
		speed = 9.0;
		break;
	case(Bullet_SmallRocket):
		damagePiercing = 8;
		damageRadius = 5;
		inaccuracy = 6;
		deathFrame = Bullet_RocketHit;
		numFrames = 16;
		numDeathFrames = 5;
		damage = 30;//50
		speed = 4.0;
		break;
	case(Bullet_SmallRocketHeavy):
		damagePiercing = 12;
		damageRadius = 5;
		inaccuracy = 6;
		deathFrame = Bullet_RocketHit;
		numFrames = 16;
		numDeathFrames = 5;
		damage = 30;//50
		speed = 4.0;
		break;
	case(Bullet_Sonic):
		damagePiercing = 4;
		damageRadius = 8;
		inaccuracy = 4;
		deathFrame = Bullet_Sonic;
		numFrames = 1;
		numDeathFrames = 1;
		damage = 40;//50
		speed = 4.0;
		graphic =  copySurface(graphic);	//make a copy of the image
		break;
	default:
		damage = 500;
		break;
	}

	imageW = graphic->w/numFrames;

	destination.x = newRealDestination->x + getRandomInt(-inaccuracy, inaccuracy);
	destination.y = newRealDestination->y + getRandomInt(-inaccuracy, inaccuracy);

	if (itemID == Bullet_Sonic)
	{
		int diffX = destination.x - newRealLocation->x,
			diffY = destination.y - newRealLocation->y;

		double ratio;
		double square_root;

		if ((diffX == 0) && (diffY == 0))
			diffY = SONICTANK_WEAPONRANGE*BLOCKSIZE;
		
		square_root = sqrt((double)(diffX*diffX + diffY*diffY));
		ratio = (SONICTANK_WEAPONRANGE*BLOCKSIZE)/square_root;
		destination.x = newRealLocation->x + (int)(((double)diffX)*ratio);
		destination.y = newRealLocation->y + (int)(((double)diffY)*ratio);
	}
	realX = (double)newRealLocation->x;
	realY = (double)newRealLocation->y;
	source.x = newRealLocation->x;
	source.y = newRealLocation->y;
	location.x = newRealLocation->x/BLOCKSIZE;
	location.y = newRealLocation->y/BLOCKSIZE;

	destAngle = dest_angle(newRealLocation, &destination);
	drawnAngle = (int)((double)numFrames*destAngle/256.0);
	angle = destAngle;

	frameTime = 5;
	frameTimer = -1;

	xOffset = imageW/2;                    //this is where it actually draws the graphic
	yOffset = imageH/2;                    //cause it will draw from top left, not middle

	xSpeed = speed * cos(destAngle * conv2char);
	ySpeed = speed * -sin(destAngle * conv2char);
}
BulletClass::~BulletClass()
{
}
void BulletClass::blitToScreen()
{
	SDL_Rect dest, source;
	if (!destroyed)
	{
		dest.x = getDrawnX();
		dest.y = getDrawnY();
		dest.w = source.w = imageW;
		dest.h = source.h = imageH;
		source.x = 0;
		if (numFrames > 1)
			source.x = drawnAngle * imageW;
		source.y = 0;

		if (itemID == Bullet_Sonic)
		{
			SDL_Surface *mask = (SDL_Surface*)dataFile[Bullet_Sonic].dat;
			if (mask->format->BitsPerPixel == 8)
			{
				if ((!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
					&& (!SDL_MUSTLOCK(mask) || (SDL_LockSurface(mask) == 0))
					&& (!SDL_MUSTLOCK(graphic) || (SDL_LockSurface(graphic) == 0)))
				{
					unsigned char	*maskPixels = (unsigned char*)mask->pixels,
									*screenPixels = (unsigned char*)screen->pixels,
									*surfacePixels = (unsigned char*)graphic->pixels;
					int	i,j, x,y,
                        maxX = fullscreenMode ? screen->w : gameBarPos.x;
					for (i = 0; i < dest.w; i++)
					for (j = 0; j < dest.h; j++)
					{
						if (maskPixels[i + j*mask->pitch] == 0)	//not masked, direct copy
						{
							x = i;
							y = j;
						}
						else
						{
							x = i + getRandomInt(-3, 3);
							y = j + getRandomInt(-3, 3);
						}

						if (dest.x < 0)
							dest.x = 0;
						else if (dest.x >= maxX)
							dest.x = maxX - 1;
						if (x < 0)
                            x = 0;
						else if (dest.x + x >= maxX)
							x = maxX - dest.x - 1;

						if (dest.y < 0)
							dest.y = 0;
						else if (dest.y >= screen->h)
							dest.y = screen->h - 1;
						if (y < 0)
                            y = 0;
						else if (dest.y + y >= screen->h)
							x = screen->h - dest.y - 1;

						surfacePixels[i + j*graphic->pitch] = screenPixels[dest.x + x + (dest.y + y)*screen->pitch];
					}

					if (SDL_MUSTLOCK(graphic))
						SDL_UnlockSurface(graphic);
					if (SDL_MUSTLOCK(mask))
						SDL_UnlockSurface(mask);
					if (SDL_MUSTLOCK(screen))
						SDL_UnlockSurface(screen);
				}
			}
		}//end of if sonic

		SDL_BlitSurface(graphic, &source, screen, &dest);
	}
	else if (numDeathFrames > 1)
	{
		source.y = 0;

		if (itemID == Bullet_LargeRocket)
		{
			dest.w = source.w = imageW;
			dest.h = source.h = imageH;
			source.x = source.w * deathFrame;

			for(int i = 0; i < 5; i++)
			for(int j = 0; j < 5; j++)
			if (((i != 0) && (i != 4)) || ((j != 0) && (j != 4)))
			{
				dest.x = getDrawnX() + (i - 2)*BLOCKSIZE - BLOCKSIZE/2;
				dest.y = getDrawnY() + (j - 2)*BLOCKSIZE - BLOCKSIZE/2;
				SDL_BlitSurface(deathGraphic[i][j], &source, screen, &dest);
			}
		}
		else
		{
			dest.x = getDrawnX();
			dest.y = getDrawnY();
			dest.w = source.w = graphic->w/numDeathFrames;
			dest.h = source.h = graphic->h;
			source.x = source.w * deathFrame;

			if (itemID == Bullet_DRocket)
				SDL_SetPalette(graphic, SDL_LOGPAL, &palette->colors[houseColour[owner->getHouse()]], COLOUR_HARKONNEN, 7);
			SDL_BlitSurface(graphic, &source, screen, &dest);
		}
	}
	else
		frameTimer = 0;
}
void BulletClass::updatePosition()
{
	if (shooter && shooter->wasDestroyed())
		shooter = NULL;

	if (!destroyed)
	{
		COORDTYPE oldLocation = location;

		realX += xSpeed;  //keep the bullet moving by its current speeds
		realY += ySpeed;
		location.x = (short)(realX/BLOCKSIZE);
		location.y = (short)(realY/BLOCKSIZE);

		if ((location.x < -5) || (location.x >= map->sizeX + 5) || (location.y < -5) || (location.y >= map->sizeY + 5))
			frameTimer = 0;	//its off the screen, kill it
		else
		{
			if (distance_from(source.x, source.y, realX, realY) >= distance_from(&source, &destination))
			{
				realX = destination.x;
				realY = destination.y;
				destroy();
			}
			else if (itemID == Bullet_Sonic)
			{
				if ((location.x != oldLocation.x) || (location.y != oldLocation.y))
				{
					COORDTYPE realPos = { (short)realX, (short)realY };
					map->damage(shooter, owner, &realPos, itemID, damage, damagePiercing, damageRadius, false);
				}
			}
			else if (map->cellExists(&location) && map->getCell(&location)->hasAGroundObject() && map->getCell(&location)->getGroundObject()->isAStructure())
			{
				if (groundBlocked)
					destroy();
			}
		}
	}
	if (frameTimer > 0)		//death frame has started
	{
		if (frameTimer == 1)
		{
			deathFrame++;
			if (deathFrame < numDeathFrames)
				frameTimer = frameTime;
		}
		frameTimer--;
	}
}

void BulletClass::destroy()
{
	if (!destroyed)
	{
		COORDTYPE realPos = { (short)realX, (short)realY };

		if (itemID == Bullet_LargeRocket)
		{
			int i, j, num;
			for(i = 0; i < 5; i++)
			for(j = 0; j < 5; j++)
			if (((i != 0) && (i != 4)) || ((j != 0) && (j != 4)))
			{
				num = getRandomInt(0, 1) ? Object_LargeExplosion1 : Object_LargeExplosion2;	//50% chance of either
				deathGraphic[i][j] = (SDL_Surface*)dataFile[num].dat;
				realPos.x = (short)realX + (i - 2)*BLOCKSIZE;
				realPos.y = (short)realY + (j - 2)*BLOCKSIZE;

				map->damage(shooter, owner, &realPos, itemID, damage, damagePiercing, damageRadius, airAttack);
				if (deathSound != NONE)
					soundPlayer->playSfxSound(deathSound);
			}

			imageW = deathGraphic[0][1]->w/numDeathFrames;
			imageH = deathGraphic[0][1]->h;
			xOffset = (imageW - BLOCKSIZE)/2;		    //this is where it actually draws the graphic
			yOffset = (imageH - BLOCKSIZE)/2;		    //cause it draws at top left, not middle
		}
		else
		{
			if (itemID == Bullet_Sonic)
				SDL_FreeSurface(graphic);
			graphic = (SDL_Surface*)(dataFile[deathFrame].dat);
			xOffset = (graphic->w/numDeathFrames)/2;		    //this is where it actually draws the graphic
			yOffset = (graphic->h)/2;		    //cause it draws at top left, not middle

			map->damage(shooter, owner, &realPos, itemID, damage, damagePiercing, damageRadius, airAttack);
			if (deathSound != NONE)
				soundPlayer->playSfxSound(deathSound);
		}

		deathFrame = 0;
		destroyed = true;
		frameTimer = frameTime;
	}
}

void BulletClass::load(FILE* fp)
{
	int x, y;

	fread(&x, sizeof(x), 1, fp);
	if ((x >= 0) && (x < MAX_PLAYERS))
		owner = player[x];
	else
		owner = player[0];

	fread(&x, sizeof(x), 1, fp);
	fread(&y, sizeof(y), 1, fp);

	location.x = x;
	location.y = y;
}

void BulletClass::save(FILE* fp)
{
	int x, y;
	if (shooter)
		x = shooter->getObjectID();
	else
		x = NONE;
	fwrite(&x, sizeof(x), 1, fp);

	fwrite(&source, sizeof(source), 1, fp);
	fwrite(&destination, sizeof(destination), 1, fp);

	x = itemID;
	fwrite(&x, sizeof(x), 1, fp);

	if (airAttack)
		x = 1;
	else
		x = 0;
	fwrite(&x, sizeof(x), 1, fp);

	//loaded by the bullet itself
	x = owner->getPlayerNumber();
	fwrite(&x, sizeof(x), 1, fp);

	x = location.x;
	y = location.y;
	fwrite(&x, sizeof(x), 1, fp);
	fwrite(&y, sizeof(y), 1, fp);
}

