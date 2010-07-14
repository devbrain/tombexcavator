#include "../sand.h"

/*

void catch_stdin()

{

	if (focusedWidget != NULL && focusedWidget->isAnEntry())

	{

		char input[2];

		input[1] = '\0';

		//while(!feof(stdin))

		{

			input[0] = fgetc(stdin);

			((Entry*)focusedWidget)->setText(input);

		}

	}

}

*/
void putpixel(SDL_Surface *surface, int x, int y, Uint32 colour)
{
	if (x >= 0 && x < screen->w && y >=0 && y < screen->h)
	{
		int bpp = surface->format->BytesPerPixel;
		/* Here p is the address to the pixel want to set */
		Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

		switch(bpp) {
		case 1:
			*p = colour;
			break;

		case 2:
			*(Uint16 *)p = colour;
			break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				p[0] = (colour>> 16) & 0xff;
				p[1] = (colour>> 8) & 0xff;
				p[2] = colour& 0xff;
			} else {
				p[0] = colour& 0xff;
				p[1] = (colour>> 8) & 0xff;
				p[2] = (colour>> 16) & 0xff;
			}
			break;

		case 4:
			*(Uint32 *)p = colour;
			break;
		}
	}
}

void hlineNoLock(SDL_Surface *surface, int x1, int y, int x2, Uint32 colour)
{
	int	min = x1,
		max = x2;

	if (min > max)
	{
		int temp = max;
		max = min;
		min = temp;
	}

	for (int i = min; i <= max; i++)
		putpixel(surface, i, y, colour);
}
void drawhline(SDL_Surface *surface, int x1, int y, int x2, Uint32 colour)
{
	if (!SDL_MUSTLOCK(surface) || (SDL_LockSurface(surface) == 0))
	{
		hlineNoLock(surface, x1, y, x2, colour);

		if (SDL_MUSTLOCK(surface))
			SDL_UnlockSurface(surface);
	}
}

void drawrect(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 colour)
{
	if (!SDL_MUSTLOCK(surface) || (SDL_LockSurface(surface) == 0))
	{
		int	min = x1,
			max = x2;
		if (min > max)
		{
			int temp = max;
			max = min;
			min = temp;
		}

		for (int i = min; i <= max; i++)
		{
			putpixel(surface, i, y1, colour);
			putpixel(surface, i, y2, colour);
		}
			
		min = y1+1;
		max = y2;
		if (min > max)
		{
			int temp = max;
			max = min;
			min = temp;
		}

		for (int j = min; j < max; j++)
		{
			putpixel(surface, x1, j, colour);
			putpixel(surface, x2, j, colour);
		}

		if (SDL_MUSTLOCK(surface))
			SDL_UnlockSurface(surface);
	}
}

void mapImageHouseColourBase(SDL_Surface* graphic, int house, int baseCol)

{

	if (settings.doubleBuffered && (graphic->format->BitsPerPixel == 8))

	{

		if (!SDL_MUSTLOCK(graphic) || (SDL_LockSurface(graphic) >= 0))

		{

			Uint8	*pixel;

			int		i, j;



			for (i = 0; i < graphic->w; i++)

			for (j = 0; j < graphic->h; j++)

			{

				pixel = &((Uint8*)graphic->pixels)[j * graphic->pitch + i];

				if ((*pixel >= baseCol) && (*pixel < baseCol + 7))

					*pixel = *pixel - baseCol + houseColour[house];

			}

			SDL_UnlockSurface(graphic);

		}

	}

	else

		SDL_SetColors(graphic, &palette->colors[houseColour[house]], baseCol, 7);

}

void mapImageHouseColour(SDL_Surface* graphic, int house)
{
	mapImageHouseColourBase(graphic, house, COLOUR_HARKONNEN);
}


void clearMessage()

{

	int i;

	for (i = 0; i < MESSAGE_BUFFER_SIZE; i++)

	{

		messageBuffer[i][0] = '\0';

		messageTimer[i] = 0;

	}

}


void showMessage(char* newMessage)

{
	if(!edit_mode)
	{
	/*MessagesBox knows if message is already in queue*/
	messageBox.addMessage(newMessage);
	}
	else
	{
	bool found = false;

	int i;

	for (i = 0; i < MESSAGE_BUFFER_SIZE; i++)

	if (strlen(messageBuffer[i]) > 0)

	{

		if (strcmp(messageBuffer[i], newMessage) == 0)

			found = true;

		i = MESSAGE_BUFFER_SIZE;

	}



	if (!found)

	{

		for (i = MESSAGE_BUFFER_SIZE - 1; i > 0; i--)

		{

			strcpy(messageBuffer[i], messageBuffer[i-1]);

			messageTimer[i] = messageTimer[i-1];

		}

		strcpy(messageBuffer[0], newMessage);

		messageTimer[0] = 500;

	}
	};

}

SDL_Surface* copySurface(SDL_Surface* inSurface)
{
	//return SDL_DisplayFormat(inSurface);
	return SDL_ConvertSurface(inSurface, inSurface->format, inSurface->flags);
}

SDL_Surface* scaleSurface(SDL_Surface *surf, double ratio)
{
SDL_Surface *scaled = 
	SDL_CreateRGBSurface(
			SDL_HWSURFACE,
			surf->w * ratio,/*width*/
			surf->h * ratio,/*height*/
			32,
			0,0,0,0);/*r,g,b,*/

SDL_LockSurface(scaled);
SDL_LockSurface(surf);

int X2 = (int)(surf->w * ratio); 
int Y2 = (int)(surf->h * ratio); 

for(int x = 0;x < X2;++x)
	for(int y = 0;y < Y2;++y)
		putpixel(scaled,x,y,
		getpixel(surf,x/ratio,y/ratio));

SDL_UnlockSurface(scaled);
SDL_UnlockSurface(surf);

return scaled;
};

