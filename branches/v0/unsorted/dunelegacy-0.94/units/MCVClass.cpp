#include "../sand.h"
#include "../Items.h"

MCVClass::MCVClass(PlayerClass* newOwner) : GroundUnit(newOwner)
{
	baseID = itemID = Unit_MCV;

	armour = 7;
	speed = 1.0/8.0;
	maxHealth = 600;
	health = maxHealth;
	radius = 9;
	viewRange = MCV_VIEWRANGE;
	weaponRange = MCV_WEAPONRANGE;

	attackMode = SCOUT;
	canAttackStuff = false;

	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;
}

MCVClass::~MCVClass()
{
}

void MCVClass::deploy()
{
	if (map->okayToBuildExclusive(getX(), getY(), 2, 2))//2x2 is size of construction yard
	{
		setVisible(VIS_ALL, false);
		getOwner()->netPlaceStructure(getObjectID(), getX(), getY(), Structure_ConstructionYard, getX(), getY());
		netDestroy();
	}
	else
		showMessage("You cannot deploy here.");
}

void MCVClass::netDeploy()
{
	deploy();
}

