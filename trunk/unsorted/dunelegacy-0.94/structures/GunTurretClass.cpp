#include "../sand.h"
#include "../Items.h"

GunTurretClass::GunTurretClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_GunTurret;

	structureSize.x = 1;
	structureSize.y = 1;

	angle = getRandomInt(0, 7);
	canAttackStuff = true;
	drawnAngle = round(angle);
	attackMode = DEFENSIVE;
	turnSpeed = 0.01;

	attackSound = Sound_gunTurret;
	bulletType = Bullet_Shell;
	primaryWeaponTimer = 0;
	primaryWeaponReloadTime = 100;

	findTargetTimer = 0;

	maxHealth = GUNTURRET_HITPOINTS;
	powerRequirement = 10;
	health = maxHealth;
	viewRange = GUNTURRET_VIEWRANGE;
	weaponRange = GUNTURRET_WEAPONRANGE;
			
	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_BuildingTurret].dat);
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

GunTurretClass::~GunTurretClass()
{
}
void GunTurretClass::doSpecificStuff()
{
	if (target)
	{
		if (!canAttack(target))
			setTarget(NULL);
		else if (targetInWeaponRange())
		{
			destAngle = dest_angle(&location, &target->getClosestPoint(&location));
			wantedAngle = round(8.0/256.0*destAngle);
			if (wantedAngle == 8)
				wantedAngle = 0;

			if (angle != wantedAngle)
				turn();
			if (drawnAngle == wantedAngle)
				attack();
		}
		else
			target = NULL;
	}
	else if ((attackMode != SCOUT) && (findTargetTimer == 0))
	{
		target = findTarget();
		findTargetTimer = 100;
	}

	if (findTargetTimer > 0) findTargetTimer--;
	if (primaryWeaponTimer > 0) primaryWeaponTimer--;
}

void GunTurretClass::handleActionCommand(int xPos, int yPos)
{
	if (map->cellExists(xPos, yPos))
	{
		ObjectClass* tempObject = map->getCell(xPos, yPos)->getObject();
		setTarget(tempObject);

		netDoCommand(tempObject, NULL, false);
	}
}

void GunTurretClass::setTurretAngle(int newAngle)
{
	if ((newAngle >=0) && (newAngle <= 7))
	{
		drawnAngle = newAngle;
		angle = (double)newAngle;
	}
}

void GunTurretClass::turn()
{
	if (angle > wantedAngle)
	{
		angleRight = angle - wantedAngle;
		angleLeft = fabs(8-angle)+wantedAngle;
	}
	else if (angle < wantedAngle)
	{
		angleRight = abs(8-wantedAngle) + angle;
		angleLeft = wantedAngle - angle;
	}

	if (angleLeft <= angleRight)
		turnLeft();
	else
		turnRight();
}

void GunTurretClass::turnLeft()
{
	angle += turnSpeed;
	if (angle >= 7.5)	//must keep drawnangle between 0 and 7
		angle -= 8.0;
	drawnAngle = round(angle);
	animFrame = round(angle);
}

void GunTurretClass::turnRight()
{
	angle -= turnSpeed;
	if (angle < -0.5)	//must keep drawnangle between 0 and 7
		angle += 8;
	drawnAngle = round(angle);
	animFrame = round(angle);
}

void GunTurretClass::attack()
{
	if (primaryWeaponTimer == 0)
	{
		COORDTYPE centrePoint, targetCentrePoint;
		getCentrePoint(&centrePoint);
		target->getClosestCentrePoint(&targetCentrePoint, &location);
		bulletList->insertLast(new BulletClass(this, &centrePoint, &targetCentrePoint, bulletType, target->isAFlyingUnit()));
		soundPlayer->playSoundAt(attackSound, &location);

		primaryWeaponTimer = primaryWeaponReloadTime;
	}
}

