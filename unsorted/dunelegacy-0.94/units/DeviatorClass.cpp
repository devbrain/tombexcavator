#include "../sand.h"
#include "../Items.h"

DeviatorClass::DeviatorClass(PlayerClass* newOwner) : LauncherClass(newOwner)
{
	itemID = Unit_Deviator;
	baseID = Unit_TankBase;

	armour = 5;
	speed = 0.5;
	maxHealth = DEVIATOR_HITPOINTS;
	health = DEVIATOR_HITPOINTS;
	radius = 6;
	viewRange = DEVIATOR_VIEWRANGE;
	weaponRange = DEVIATOR_WEAPONRANGE;

	numWeapons = 1;
	bulletType[0] = Bullet_DRocket;
	primaryWeaponReloadTime = 285;

	numAttackSounds = 1;
	attackSound[0] = Sound_rocket;
}

DeviatorClass::~DeviatorClass()
{
}

bool DeviatorClass::canAttack(ObjectClass* object)
{
	if (!object->wasDestroyed() && !object->isAStructure() && ((object->getOwner()->getTeam() != owner->getTeam()) || object->getItemID() == Unit_Sandworm) && object->isVisible(getOwner()->getTeam()))
		return true;
	else
		return false;
}

