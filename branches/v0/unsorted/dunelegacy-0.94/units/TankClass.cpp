#include "../sand.h"

#include "../Items.h"



TankClass::TankClass(PlayerClass* newOwner) : TrackedUnit(newOwner)

{

	itemID = Unit_Tank;

	baseID = Unit_TankBase;

	

	turreted = true;

	setTurretAngle(getRandomInt(0, 7));

	closeTarget = NULL;



	armour = 10;

	maxHealth = 400;

	health = maxHealth;

	radius = 6;

	speed = 0.5;

	turretTurnSpeed = 0.0625;

	viewRange = TANK_VIEWRANGE;

	weaponRange = TANK_WEAPONRANGE;

	

	numWeapons = 1;

	bulletType[0] = Bullet_Shell;

	primaryWeaponReloadTime = 150;



	graphic = (SDL_Surface*)(dataFile[baseID].dat);

	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());

	turretGraphic = (SDL_Surface*)(dataFile[itemID].dat);

	imageW = graphic->w/NUM_ANGLES;

	imageH = graphic->h;

	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place

	yOffset = (imageH - BLOCKSIZE)/2;



	numAttackSounds = 1;

	attackSound[0] = Sound_smallExplosion;

}

TankClass::~TankClass()

{

	//SDL_FreeSurface(turretGraphic);

}



void TankClass::blitToScreen()

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

		dest.x = getDrawnX();

		dest.y = getDrawnY();

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



void TankClass::navigate()
{
	if(moving)//change the turret angle so it faces the direction we are moving in
	{
		targetAngle = round(8.0/256.0*dest_angle(&location, &destination));

		if (targetAngle == 8)

			targetAngle = 0;
	}

	GroundUnit::navigate();
}



void TankClass::engageTarget()

{

	if (target)

	{

		if (target->wasDestroyed()

		|| ((!canAttack(target)

				|| (!forced && !isInAttackModeRange(target)))

			&& !targetFriendly))

		{

			findTargetTimer = 0;

			forced = false;

			setTarget(NULL);



			if (attackMode != DEFENSIVE)

				setGuardPoint(&location);

			setDestination(&guardPoint);

			nextSpotFound = false;

			pathList.clearList();

		}

		else

		{

			COORDTYPE targetLocation = target->getClosestPoint(&location);

			targetDistance = blockDistance(&location, &targetLocation);



			if ((destination.x != targetLocation.x)	//targets location has moved, recalculate path

				&& (destination.y != targetLocation.y))

				pathList.clearList();



			if (forced)

				setDestination(&targetLocation);



			targetAngle = round(8.0/256.0*dest_angle(&location, &targetLocation));

			if (targetAngle == 8)

				targetAngle = 0;



			if (targetDistance <= weaponRange)

			{

				if (!targetFriendly)

				{

					if (target->isInfantry() && (forced || (attackMode == AGGRESSIVE) || (attackMode == DEFENSIVE)))

						setDestination(&targetLocation);

					else

					{

                        setDestination(&location);

						pathList.clearList();

						nextSpotFound = false;

					}

					if (drawnTurretAngle == targetAngle)

						attack();

				}

			}

			else

				setDestination(&targetLocation);

		}

	}



	if (closeTarget)	//if has a close temporary target

	{

		if (closeTarget->wasDestroyed()

			|| !canAttack(closeTarget))

			/*closeTarget->isAStructure()

				&& ((UnitClass*)closeTarget)->wasDeviated()

				&& (closeTarget->getOwner()->getTeam() == owner->getTeam())))*/

			closeTarget = NULL;

		else if (!target || targetFriendly || (targetDistance > weaponRange))

		{

			COORDTYPE targetLocation = closeTarget->getClosestPoint(&location);

			targetDistance = blockDistance(&location, &targetLocation);



			if (targetDistance > weaponRange)

				closeTarget = NULL;



			if (closeTarget)

			{

				targetAngle = round(8.0/256.0*dest_angle(&location, &targetLocation));

				if (targetAngle == 8)

					targetAngle = 0;



				if (targetDistance <= weaponRange)

				{

					if (drawnTurretAngle == targetAngle)

					{

						ObjectClass* temp = target;

						target = closeTarget;

						attack();

						target = temp;

					}

				}

			}

		}

	}

}



void TankClass::netDoConsistancy()

{

	if (settings.playerType == SERVER)

		netServerConsistancyMessage(objectID, &location, drawnAngle, drawnTurretAngle);

}



void TankClass::setTurretAngle(int newAngle)

{

	if ((newAngle >= 0) && (newAngle <= 7))

	{

		drawnTurretAngle = newAngle;

		turretAngle = (double)drawnTurretAngle;

	}

}



void TankClass::targeting()

{

	

	if (active && !moving && (attackMode != SCOUT) && (findTargetTimer == 0))

	{

		if (settings.playerType != CLIENT)

		{

			if (!forced)

			{

				if (!target)

				{

					target = findTarget();

					if (target && isInAttackModeRange(target))

					{

						netDoAttack(target);

						pathList.clearList();

						nextSpotFound = false;

						speedCap = NONE;

					}

					else

					{

						target = NULL;

						if (attacking && (settings.playerType != CLIENT))

						{

							StructureClass* closestStructure = (StructureClass*)findClosestTargetStructure(this);

							if (closestStructure)

								netDoCommand(NULL, &closestStructure->getClosestPoint(&location), false);

							else

							{

								UnitClass* closestUnit = (UnitClass*)findClosestTargetUnit(this);

								if (closestUnit)

									netDoCommand(NULL, closestUnit->getLocation(), false);

							}

						}

					}

					findTargetTimer = 100;

				}

			}

		}

		

		if (!closeTarget)

		{

			closeTarget = findTarget();	//find a temporary target

			findTargetTimer = 100;

		}

	}



	if (target || closeTarget)

		engageTarget();

}



void TankClass::turn()

{



	double	angleLeft = 0,

			angleRight = 0;



	if (!moving)

	{

		if (nextSpotAngle != NONE)

		{

			if (angle > nextSpotAngle)

			{

				angleRight = angle - nextSpotAngle;

				angleLeft = fabs(8-angle) + nextSpotAngle;

			}

			else if (angle < nextSpotAngle)

			{

				angleRight = abs(8-nextSpotAngle) + angle;

				angleLeft = nextSpotAngle - angle;

			}



			if (angleLeft <= angleRight)

				turnLeft();

			else

				turnRight();

		}

	}



	if (targetAngle != NONE)

	{

		if (turretAngle > targetAngle)

		{

			angleRight = turretAngle - targetAngle;

			angleLeft = fabs(8-turretAngle) + targetAngle;

		}

		else if (turretAngle < targetAngle)

		{

			angleRight = abs(8-targetAngle) + turretAngle;

			angleLeft = targetAngle - turretAngle;

		}



		if (angleLeft <= angleRight)

			turnTurretLeft();

		else

			turnTurretRight();

	}

}



void TankClass::turnTurretLeft()

{

	turretAngle += turretTurnSpeed;

	if (turretAngle >= 7.5)

		turretAngle -= 8.0;

	drawnTurretAngle = round(turretAngle);	    //decrease the turretAngle by the turn speed

}



void TankClass::turnTurretRight()

{

	turretAngle -= turretTurnSpeed;	    //increase the turretAngle by the turn speed

	if (turretAngle < -0.5)

		turretAngle += 8;

	drawnTurretAngle = round(turretAngle);	   //decrease the turretAngle by the turn speed



}

