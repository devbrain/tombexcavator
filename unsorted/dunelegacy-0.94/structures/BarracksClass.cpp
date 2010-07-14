#include "../sand.h"
#include "StructureClass.h"
#include "BuilderClass.h"
#include "BarracksClass.h"

BarracksClass::BarracksClass(PlayerClass* newOwner) : BuilderClass(newOwner)
{
	itemID = Structure_Barracks;

	structureSize.x = 2;
	structureSize.y = 2;

	builder = true;

	maxHealth = BARRACKS_HITPOINTS;
	powerRequirement = 20;
	health = BARRACKS_HITPOINTS;
	viewRange = BARRACKS_VIEWRANGE;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic =  copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

BarracksClass::~BarracksClass()
{
}

void BarracksClass::checkSelectionList()
{
	BuilderClass::checkSelectionList();

	insertItem(Unit_Infantry);

	checkMinMaxSelection();
}
void BarracksClass::doSpecificStuff()
{
	buildUpdate();
}

