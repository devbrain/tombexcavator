#include "../sand.h"
#include "../Items.h"

RadarClass::RadarClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_Radar;

	structureSize.x = 2;
	structureSize.y = 2;

	maxHealth = RADAR_HITPOINTS;
	powerRequirement = 20;
	health = RADAR_HITPOINTS;
	viewRange = RADAR_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w/4;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle

	animFrames = 4;
	animFrame = 0;
	isAnimating = 1;
}

RadarClass::~RadarClass()
{
}

void RadarClass::doSpecificStuff()
{

}

void RadarClass::destroy()
{
	StructureClass::destroy();
	if(!getOwner()->hasRadar())
		getOwner()->changeRadar(false);
}

