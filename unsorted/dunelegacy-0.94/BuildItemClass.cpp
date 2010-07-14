#include "sand.h"

BuildItemClass::BuildItemClass()
{
	itemID = Unknown;
	structureSize.x = structureSize.y = 0;
	description[0] = '\0';
}

BuildItemClass::BuildItemClass(int newItemID)
{
	itemID = newItemID;
	structureSize.x = structureSize.y = 0;
	description[0] = '\0';

	int picID;
	switch (itemID)
	{
	case (Structure_Barracks):
		bListPriority = 9;
		cost = 300;
		picID = Picture_Barracks;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "Barracks");
		sprintf(description, "%s: %d", name, round(cost));
		break;
	case (Structure_ConstructionYard):
		bListPriority = 17;
		cost = 20000;
		picID = Picture_ConstructionYard;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "Construction Yard");
		sprintf(description, "Construction Yard: %d", round(cost));
		break;
	case (Structure_GunTurret):
		bListPriority = 7;
		cost = 125;//350;
		picID = Picture_GunTurret;
		structureSize.x = 1;
		structureSize.y = 1;
		sprintf(name, "Gun Turret");
		sprintf(description, "Gun Turret: %d", round(cost));
		break;
	case (Structure_HeavyFactory):
		bListPriority = 12;
		cost = 600;
		picID = Picture_HeavyFactory;
		structureSize.x = 3;
		structureSize.y = 2;
		sprintf(name, "Heavy Factory");
		sprintf(description, "Heavy Factory: %d", round(cost));
		break;
	case (Structure_HighTechFactory):
		bListPriority = 13;
		cost = 500;//600;
		picID = Picture_HighTechFactory;
		structureSize.x = 3;
		structureSize.y = 2;
		sprintf(name, "High Tech Factory");
		sprintf(description, "%s: %d", name, round(cost));
		break;
	case (Structure_IX):
		bListPriority = 16;
		cost = 500;
		picID = Picture_IX;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "House of IX");
		sprintf(description, "House of IX: %d", round(cost));
		break;
	case (Structure_LightFactory):
		bListPriority = 11;
		cost = 400;//500;
		picID = Picture_LightFactory;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "Light Factory");
		sprintf(description, "Light Factory: %d", round(cost));
		break;
	case (Structure_Palace):
		bListPriority = 17;
		cost = 999;//2000;
		picID = Picture_Palace;
		structureSize.x = 3;
		structureSize.y = 3;
		sprintf(name, "Palacey");
		sprintf(description, "Palace: %d", round(cost));
		break;
	case (Structure_Radar):
		bListPriority = 6;
		cost = 400;
		picID = Picture_Radar;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "Radar");
		sprintf(description, "Radar: %d", round(cost));
		break;
	case (Structure_Refinery):
		bListPriority = 4;
		cost = 400;//1600;
		picID = Picture_Refinery;
		structureSize.x = 3;
		structureSize.y = 2;
		sprintf(name, "Refinery");
		sprintf(description, "Refinery: %d", round(cost));
		break;
	case (Structure_RepairYard):
		bListPriority = 14;
		cost = 700;
		picID = Picture_RepairYard;
		structureSize.x = 3;
		structureSize.y = 2;
		sprintf(name, "Repair Yard");
		sprintf(description, "Repair Yard: %d", round(cost));
		break;
	case (Structure_RocketTurret):
		bListPriority = 8;
		cost = 250;//500;
		picID = Picture_RocketTurret;
		structureSize.x = 1;
		structureSize.y = 1;
		sprintf(name, "Rocket Turret");
		sprintf(description, "Rocket Turret: %d", round(cost));
		break;
	case (Structure_Silo):
		bListPriority = 5;
		cost = 150;
		picID = Picture_Silo;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "Silo");
		sprintf(description, "Silo: %d", round(cost));
		break;
	case (Structure_StarPort):
		bListPriority = 15;
		cost = 500;//1000;
		picID = Picture_StarPort;
		structureSize.x = 3;
		structureSize.y = 3;
		sprintf(name, "Star Port");
		sprintf(description, "Star Port: %d", round(cost));
		break;
	case (Structure_Slab1):
		bListPriority = 0;
		cost = 5;
		picID = Picture_Slab1;
		structureSize.x = 1;
		structureSize.y = 1;
		sprintf(name, "1x1 Concrete Slab");
		sprintf(description, "1x1 Concrete Slab: %d", round(cost));
		break;
	case (Structure_Slab4):
		bListPriority = 1;
		cost = 20;
		picID = Picture_Slab4;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "2x2 Concrete Slab");
		sprintf(description, "2x2 Concrete Slab: %d", round(cost));
		break;
	case (Structure_Wall):
		bListPriority = 2;
		cost = 50;
		picID = Picture_Wall;
		structureSize.x = 1;
		structureSize.y = 1;
		sprintf(name, "Wall");
		sprintf(description, "Wall: %d", round(cost));
		break;	
	case (Structure_WindTrap):
		bListPriority = 3;
		cost = 300;
		picID = Picture_WindTrap;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "Wind Trap");
		sprintf(description, "Wind Trap: %d", round(cost));
		break;	
	case (Structure_WOR):
		bListPriority = 10;
		cost = 400;
		picID = Picture_WOR;
		structureSize.x = 2;
		structureSize.y = 2;
		sprintf(name, "WOR");
		sprintf(description, "WOR: %d", round(cost));
		break;

	case (Unit_Carryall):

		bListPriority = 1;

		cost = 800;

		picID = Picture_Carryall;

		sprintf(name, "Carryall");

		sprintf(description, "Carryall: %d", round(cost));

		break;
	case (Unit_Devastator):
		bListPriority = 1;
		cost = 750;
		picID = Picture_Devastator;
		sprintf(name, "Devastator");
		sprintf(description, "Devastator: %d", round(cost));
		break;
	case (Unit_Deviator):
		bListPriority = 3;
		cost = 750;
		picID = Picture_Deviator;
		sprintf(name, "Deviator");
		sprintf(description, "Deviator: %d", round(cost));
		break;
	case (Unit_Harvester):
		bListPriority = 11;
		cost = 300;
		picID = Picture_Harvester;
		sprintf(name, "Harvester");
		sprintf(description, "Harvester: %d", round(cost));
		break;
	case (Unit_Infantry):
		bListPriority = 10;
		cost = 100;
		picID = Picture_Infantry;
		sprintf(name, "Light Infantry");
		sprintf(description, "Light Infantry: %d", round(cost));
		break;
	case (Unit_Launcher):
		bListPriority = 5;
		cost = 600;
		picID = Picture_Launcher;
		sprintf(name, "Rocket Launcher");
		sprintf(description, "Rocket Launcher: %d", round(cost));
		break;		
	case (Unit_MCV):
		bListPriority = 12;
		cost = 900;
		picID = Picture_MCV;
		sprintf(name, "MCV");
		sprintf(description, "MCV: %d", round(cost));
		break;

	case (Unit_Ornithopter):

		bListPriority = 1;

		cost = 700;

		picID = Picture_Ornithopter;

		sprintf(name, "Ornithopter");

		sprintf(description, "Ornithopter: %d", round(cost));

		break;
	case (Unit_Quad):
		bListPriority = 8;
		cost = 200;
		picID = Picture_Quad;
		sprintf(name, "Quad");
		sprintf(description, "Quad: %d", round(cost));
		break;
	case (Unit_SiegeTank):
		bListPriority = 4;
		cost = 600;
		picID = Picture_SiegeTank;
		sprintf(name, "Siege Tank");
		sprintf(description, "Siege Tank: %d", round(cost));
		break;
	case (Unit_SonicTank):
		bListPriority = 2;
		cost = 800;
		picID = Picture_SonicTank;
		sprintf(name, "Sonic Tank");
		sprintf(description, "Sonic Tank: %d", round(cost));
		break;
	case (Unit_Tank):
		bListPriority = 7;
		cost = 300;
		picID = Picture_Tank;
		sprintf(name, "Combat Tank");
		sprintf(description, "Combat Tank: %d", round(cost));
		break;
	case (Unit_Trike):
		bListPriority = 9;
		cost = 150;
		picID = Picture_Trike;
		sprintf(name, "Trike");
		sprintf(description, "Trike: %d", round(cost));
		break;
	case (Unit_Raider):
		bListPriority = 9;
		cost = 150;
		picID = Picture_Raider;
//		picID = Picture_Trike;
		sprintf(name, "Ordos Raider");
		sprintf(description, "Ordos Raider: %d", round(cost));
		break;
	case (Unit_Trooper):
		bListPriority = 12;
		cost = 50;
		picID = Picture_Trooper;
		sprintf(name, "Rocket Trooper");
		sprintf(description, "Rocket Trooper: %d", round(cost));
		break;
	case (Unit_Fremen):
		bListPriority = 12;
		cost = 75;
		picID = Picture_Fremen;
		sprintf(name, "Fremen Trooper");
		sprintf(description, "Fremen Trooper: %d", round(cost));
		break;
	case (Unit_Sardaukar):
		bListPriority = 12;
		cost = 75;
		picID = Picture_Sardaukar;
		sprintf(name, "Sardaukar Trooper");
		sprintf(description, "Sardaukar Trooper: %d", round(cost));
		break;
	default:
		bListPriority = 1000;
		cost = 300;
		picID = Picture_Tank;
		sprintf(name, "Combat Tank");
		sprintf(description, "Combat Tank: %d", round(cost));
		break;
	}

	graphic = (SDL_Surface*)(dataFile[picID].dat);

	numToBuild = 0;
	progress = 0;

	waitingToPlace = false;
	onHold = false;
}


BuildItemClass::~BuildItemClass()
{
}

void BuildItemClass::buildUpdate(double credits)
{
	if (progress < cost)
	{
		progress += credits;
	}
}


void BuildItemClass::decrementNumToBuild()

{

	numToBuild--;



	if (numToBuild == 0)

		onHold = false;

}



void BuildItemClass::draw(int x, int y)
{
	char text[50];

	SDL_Surface	//*percentCompleteRect,

				*textSurface;

	SDL_Rect drawLocation;



	drawLocation.x = x;

	drawLocation.y = y;

	drawLocation.w = graphic->w;

	drawLocation.h = graphic->h;

	SDL_BlitSurface(graphic, NULL, screen, &drawLocation);

	

	if (getPercentComplete() > 0)

	{

		int i, j;



		if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
		{

			for (i = 0; i < ((int)(getPercentComplete()*0.64)); i++)

			for (j = i % 2; j < 48; j+=2)

				putpixel(screen, x + i, y + j, COLOUR_BLACK);



			if (SDL_MUSTLOCK(screen))
				SDL_UnlockSurface(screen);

		}

	}



	if (waitingToPlace)

	{

		sprintf(text, "PLACE IT");

		textSurface = TTF_RenderText_Solid(font[10], text, palette->colors[COLOUR_WHITE]);

		drawLocation.x = x + (graphic->w - textSurface->w)/2;

		drawLocation.y = y + (graphic->h - textSurface->h)/2;

		drawLocation.w = textSurface->w;

		drawLocation.h = textSurface->h;

		SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);

		SDL_FreeSurface(textSurface);

	}

	else if (onHold)

	{

		sprintf(text, "ON HOLD");

		textSurface = TTF_RenderText_Solid(font[10], text, palette->colors[COLOUR_WHITE]);

		drawLocation.x = x + (graphic->w - textSurface->w)/2;

		drawLocation.y = y + (graphic->h - textSurface->h)/2;

		drawLocation.w = textSurface->w;

		drawLocation.h = textSurface->h;

		SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);

		SDL_FreeSurface(textSurface);

	}



	//show how much

	/*sprintf(text, "%d", (int)cost);

	textSurface = TTF_RenderText_Solid(font[10], text, palette->colors[COLOUR_WHITE]);

	drawLocation.x = x;

	drawLocation.y = y;

	drawLocation.w = textSurface->w;

	drawLocation.h = textSurface->h;

	SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);

	SDL_FreeSurface(textSurface);*/

	

	//show how many of this type in build queue	

	if (numToBuild)

	{

		sprintf(text, "%d", numToBuild);

		textSurface = TTF_RenderText_Solid(font[10], text, palette->colors[COLOUR_RED]);

		drawLocation.x = x + graphic->w - textSurface->w;

		drawLocation.y = y + graphic->h - textSurface->h;

		drawLocation.w = textSurface->w;

		drawLocation.h = textSurface->h;

		SDL_BlitSurface(textSurface, NULL, screen, &drawLocation);

		SDL_FreeSurface(textSurface);

	}
}

void BuildItemClass::setWaitingToPlace(bool value)
{
	waitingToPlace = value;
	if (waitingToPlace)
		progress = cost;
}

double BuildItemClass::getPercentComplete()
{
	return progress*100.0/cost;
}

/*int BuildItemClass::getCost()
{
	return (int)cost;
}*/

