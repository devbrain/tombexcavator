#include "../sand.h"
#include "../Items.h"

Frigate::Frigate(PlayerClass* newOwner) : Carryall(newOwner)
{
	baseID = itemID = Unit_Frigate;

	armour = 10;
	speed = 1.0;
	maxHealth = 10000;
	health = maxHealth;
	radius = 16;
	viewRange = 0;
	weaponRange = 0;

	frameTime = DEFAULT_UNITFRAMETIME;
	nextSpotAngle = drawnAngle;
	turnSpeed = 0.125;
	weaponRange = 0;
	viewRange = DEFAULT_UNITVIEWRANGE;

	attackMode = SCOUT;
	aDropOfferer = true;
	canAttackStuff = false;

	graphic = (SDL_Surface*)(dataFile[baseID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	imageW = graphic->w/NUM_ANGLES;
	imageH = graphic->h;
	xOffset = (imageW - BLOCKSIZE)/2;	//these are needed so image will be drawn at the right place
	yOffset = (imageH - BLOCKSIZE)/2;

	respondable = false;

	droppedOffCargo = false;
	numCargo = 0;

	owned = false;
}
Frigate::~Frigate()
{
}

