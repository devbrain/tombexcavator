#include "../sand.h"
//#include "gui.h"

Window::Window() : Container()
{
	surface = NULL;
}
Window::~Window()
{
	if(surface)
		SDL_FreeSurface(surface);
}

void Window::draw()
{
	if (visible)
	{
//		SDL_FillRect(screen, &rect, COLOUR_WHITE);
//		drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, COLOUR_BLACK);

		if (surface)
		{
			SDL_Rect dest = rect;
			dest.x += (rect.w - surfaceSrc.w)/2 + 1;
			dest.y += (rect.h - surfaceSrc.h)/2 + 1;
			SDL_BlitSurface(surface, &surfaceSrc, screen, &dest);
		}

		Container::draw();//draw children anyway
	}
	
}

void Window::setSurface(SDL_Surface* newSurface, SDL_Rect* src)
{
	if (surface)
		SDL_FreeSurface(surface);
	surface = SDL_DisplayFormat(newSurface);
	if (src == NULL)
	{
		surfaceSrc.x = surfaceSrc.y = 0;
		surfaceSrc.w = newSurface->w;
		surfaceSrc.h = newSurface->h;
	}
	else
		surfaceSrc = *src;
}

