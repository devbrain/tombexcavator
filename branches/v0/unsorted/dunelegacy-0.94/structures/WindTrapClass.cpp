#include "../sand.h"
#include "../Items.h"

WindTrapClass::WindTrapClass(PlayerClass* newOwner) : StructureClass(newOwner)
{
	itemID = Structure_WindTrap;

	structureSize.x = 2;
	structureSize.y = 2;

	maxHealth = WINDTRAP_HITPOINTS;
	powerRequirement = 0;
	health = maxHealth;
	viewRange = WINDTRAP_VIEWRANGE;
	colourTimer = 0;

	graphic = (SDL_Surface*)(dataFile[itemID].dat);
	graphic = copySurface(graphic);
	mapImageHouseColour(graphic, getOwner()->getColour());
	placeGraphic = (SDL_Surface*)(dataFile[Structure_Building22].dat);
	imageW = graphic->w;
	imageH = graphic->h/2;
	xOffset = 0;                    //this is where it actually draws the graphic
	yOffset = 0;                    //cause it draws at top left, not middle
}

WindTrapClass::~WindTrapClass()
{
}

void WindTrapClass::doSpecificStuff()
{
	int oldColourTimer = colourTimer;
	if (++colourTimer >= 350)
		colourTimer = -349;

	if(!fogged)
	if (oldColourTimer/50 != colourTimer/50)
	{
		SDL_FreeSurface(graphic);
		graphic = (SDL_Surface*)(dataFile[itemID].dat);
		graphic =  copySurface(graphic);

		mapImageHouseColour(graphic, getOwner()->getColour());

		if (settings.doubleBuffered && (graphic->format->BitsPerPixel == 8))

		{

			if (!SDL_MUSTLOCK(graphic) || (SDL_LockSurface(graphic) == 0))

			{

				Uint8	*pixel;

				int		i, j;



				for (i = 0; i < graphic->w; i++)

				for (j = 0; j < graphic->h; j++)

				{

					pixel = &((Uint8*)graphic->pixels)[j * graphic->pitch + i];

					if (*pixel == 223)

						*pixel = 128 + abs(colourTimer/50);

				}

				SDL_UnlockSurface(graphic);

			}

		}

		else
			SDL_SetColors(graphic, &graphic->format->palette->colors[128 + abs(colourTimer/50)], 223, 1);
	}

//	interfaceContainer->setVisible(true);
//	repairButton.setVisible(true);
}

