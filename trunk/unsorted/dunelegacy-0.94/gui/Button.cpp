#include "../sand.h"
//#include "gui.h"
extern Widget *focusedWidget;

Button::Button() : Widget()
{
	mouseOver = pressed = toggled = toggler = false;

	rect.w = 20;
	rect.h = 15;

	helpText[0] = '\0';
	helpTextSurface = NULL;
	text[0] = '\0';
	textSurface = NULL;

	callbackClick = NULL;
	callbackClick2 = NULL;
	callBackData = NULL;
	callbackCancel = NULL;
	callbackCancel2 = NULL;
	callBackCancelData = NULL;

	surface = NULL;
	pressedSurface = NULL;

	hasProgress = false;
	total = 100;
	progress = 0;

	textColour = COLOUR_BLACK;
	backGroundColour = COLOUR_WHITE;
	pressedTextColour = COLOUR_BLACK;
	borderColour = COLOUR_BLACK;

	transparent = false;
}

Button::~Button()
{

	//Surfaces are usually from the global surfaces data, so we dont want to free them, otherwise we'll crash
	//might want to change this...copy the surfaces in??
//	if (pressedSurface)
//		SDL_FreeSurface(surface);

//	if (surface)
//		SDL_FreeSurface(surface);

	if (textSurface)
		SDL_FreeSurface(textSurface);

	if (helpTextSurface)
		SDL_FreeSurface(helpTextSurface);
}

void Button::draw()
{
	if (visible)
	{
		if (surface && pressedSurface)
		{
			if(pressed)
			{
				SDL_Rect dest = rect;
				dest.x += (rect.w - pressedSurfaceSrc.w)/2 + 1;
				dest.y += (rect.h - pressedSurfaceSrc.h)/2 + 1;
				SDL_BlitSurface(pressedSurface, &pressedSurfaceSrc, screen, &dest);
			}
			else
			{
				SDL_Rect dest = rect;
				dest.x += (rect.w - surfaceSrc.w)/2 + 1;
				dest.y += (rect.h - surfaceSrc.h)/2 + 1;
				SDL_BlitSurface(surface, &surfaceSrc, screen, &dest);
			}
		}
		else
		{
			if(!transparent)
			{
				if (pressed)	//button has been pressed
				{
					//drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, COLOUR_WHITE);
						
					SDL_FillRect(screen, &rect, borderColour);
					
					drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, backGroundColour);
				}
				else
				{
					//SDL_FillRect(screen, &rect, COLOUR_WHITE);
					
					SDL_FillRect(screen, &rect, backGroundColour);
					
					drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, borderColour);
				}
			}
		
			
			if(surface)
			{
				SDL_Rect dest = rect;
				dest.x += (rect.w - surfaceSrc.w)/2 + 1;
				dest.y += (rect.h - surfaceSrc.h)/2 + 1;
				
				SDL_BlitSurface(surface, &surfaceSrc, screen, &dest);
			}
		}
		

		if(hasProgress)
		{
			if (getPercentComplete() > 0)
			{
				int i, j;
				
				SDL_Rect dest = rect;
				dest.x += (rect.w - surfaceSrc.w)/2 + 1;
				dest.y += (rect.h - surfaceSrc.h)/2 + 1;

				if (!SDL_MUSTLOCK(screen) || (SDL_LockSurface(screen) == 0))
				{
					for (i = 0; i < ((int)(getPercentComplete()*0.64)); i++)//should change 0.64 to work it out from rect size
					for (j = i % 2; j < rect.h + 1; j+=2)
						putpixel(screen, rect.x + i, rect.y + j, COLOUR_BLACK);

					if (SDL_MUSTLOCK(screen))
						SDL_UnlockSurface(screen);
				}
			}
		}

		if (textSurface)
		{
			SDL_Rect textLocation;
			textLocation.x = rect.x + rect.w/2 - textSurface->w/2;
			textLocation.y = rect.y + rect.h/2 - textSurface->h/2;
			SDL_BlitSurface(textSurface, NULL, screen, &textLocation);
		}

		if (helpTextSurface && mouseOver)
		{
			SDL_Rect dest;

			dest.x = rect.x - helpTextSurface->w - 12;
			dest.y = rect.y + 12;
			dest.w = helpTextSurface->w + 4;
			dest.h = helpTextSurface->h - 4;
			SDL_FillRect(screen, &dest, COLOUR_BLACK);
			drawrect(screen, dest.x, dest.y, dest.x + dest.w, dest.y + dest.h, COLOUR_YELLOW);

			dest.x = rect.x - helpTextSurface->w - 10;
			dest.y = rect.y + 10;
			dest.w = helpTextSurface->w;
			dest.h = helpTextSurface->h;
			SDL_BlitSurface(helpTextSurface, NULL, screen, &dest);
		}
	}
}

void Button::handleMotion(Uint32 x, Uint32 y)
{
	if (enabled)
	{
		if (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight()))))
		{
			mouseLocationX = x;
			mouseLocationY = y;
			mouseOver = true;
		}
		else
		{
			mouseOver = false;
			if (focusedWidget == this)
			{
				pressed = !pressed;
				focusedWidget = NULL;

				if (text[0] != '\0')
				{
					if (textSurface)
						SDL_FreeSurface(textSurface);	//free the text then change its colour
					if (pressed)
						textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[COLOUR_WHITE]);
					else
						textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);
				}
			}
		}
	}
}

void Button::handleCancelPress(Uint32 x, Uint32 y)
{
	if (enabled && (callbackCancel || callbackCancel2)
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{
		Widget::handleCancelPress(x, y);
		setFocused(true);

		pressed = !pressed;
		if (text[0] != '\0')
		{
			if (textSurface)
				SDL_FreeSurface(textSurface);	//free the text then change its colour
			if (pressed)
				textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[COLOUR_WHITE]);
			else
				textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);
		}

		soundPlayer->playSfxSound(Sound_buttonClick);
	}
}

void Button::handleCancelRelease(Uint32 x, Uint32 y)
{
	if (enabled && (callbackCancel || callbackCancel2)
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{
		if (toggler)
			toggled = !toggled;

		if (callbackCancel2)
			callbackCancel2(callBackCancelData);
		else if (callbackCancel)
			callbackCancel();

		if (!toggler || !toggled)
		{
			pressed = false;

			if (text[0] != '\0')
			{
				if (textSurface)
					SDL_FreeSurface(textSurface);	//free the text then change its colour
				textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);
			}
		}

		setFocused(false);
	}
}

void Button::handlePress(Uint32 x, Uint32 y)
{
	if (enabled && (callbackClick || callbackClick2)
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{
		Widget::handlePress(x, y);
		setFocused(true);

		pressed = !pressed;
		if (text[0] != '\0')
		{
			if (textSurface)
				SDL_FreeSurface(textSurface);	//free the text then change its colour
			if (pressed)
				textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[COLOUR_WHITE]);
			else
				textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);
		}

		soundPlayer->playSfxSound(Sound_buttonClick);
	}
}

void Button::handleRelease(Uint32 x, Uint32 y)
{
	if (enabled && (callbackClick || callbackClick2)
		&& (((x >= getX()) && (x < (getX() + getWidth())))
		&& ((y >= getY()) && (y < (getY() + getHeight())))))
	{
		if (toggler)
			toggled = !toggled;

		setFocused(false);

		if (toggler || pressed)
		{
			if (callbackClick2)
				callbackClick2(callBackData);
			else if (callbackClick)
				callbackClick();
		}

		if (!toggler || !toggled)
		{
			pressed = false;

			if (text[0] != '\0')
			{
				if (textSurface)
					SDL_FreeSurface(textSurface);	//free the text then change its colour
				textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);
			}
		}
	}
}

void Button::setCallBack(void (*newCallback)())
{
	callbackClick = newCallback;
	callbackClick2 = NULL;
}

void Button::setCallBack(void (*newCallback)(void* data), void* newData)
{
	callbackClick2 = newCallback;
	callbackClick = NULL;
	callBackData = newData;
}

void Button::setCallBackCancel(void (*newCallback)())
{
	callbackCancel = newCallback;
	callbackCancel2 = NULL;
}

void Button::setCallBackCancel(void (*newCallback)(void* data), void* newData)
{
	callbackCancel2 = newCallback;
	callbackCancel = NULL;
	callBackCancelData = newData;
}

void Button::setHelpText(char* newText)
{
	strncpy(helpText, newText, MAX_LINE);

	if (helpTextSurface)
		SDL_FreeSurface(helpTextSurface);
	helpTextSurface = TTF_RenderText_Solid(font[14], helpText, palette->colors[COLOUR_YELLOW]);
}

void Button::setSurface(SDL_Surface* newSurface, SDL_Rect* src)
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

void Button::setPressedSurface(SDL_Surface* newSurface, SDL_Rect* src)
{
	if (pressedSurface)
		SDL_FreeSurface(pressedSurface);
	pressedSurface = SDL_DisplayFormat(newSurface);
	if (src == NULL)
	{
		pressedSurfaceSrc.x = pressedSurfaceSrc.y = 0;
		pressedSurfaceSrc.w = newSurface->w;
		pressedSurfaceSrc.h = newSurface->h;
	}
	else
		pressedSurfaceSrc = *src;
}

void Button::setText(char* newText)
{
	strncpy(text, newText, MAX_LINE);

	if (textSurface)
		SDL_FreeSurface(textSurface);
	//textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[COLOUR_BLACK]);
	textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);
}

void Button::setToggled(bool toggle)
{
	toggled = pressed = toggle;

	if (text[0] != '\0')
	{
		if (textSurface)
			SDL_FreeSurface(textSurface);	//free the text then change its colour
		if (pressed)
			textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[COLOUR_WHITE]);
		else
			textSurface = TTF_RenderText_Solid(font[14], text, palette->colors[textColour]);//used to be black
	}
	if (this == focusedWidget)
		focusedWidget = NULL;
}

bool Button::textEquals(char* checkText)
{
	return (strcmp(checkText, text) == 0);
}

char* Button::getText()
{
	return text;
}

int Button::getProgress() 
{ 
	return progress;
}
void Button::setProgress(int i) 
{
	progress = i; 
}

void Button::setTotalProgress(int i)
{
	total = i; 
}

void Button::setIsProgress(bool i) 
{
	hasProgress = i; 
}

bool Button::isProgress() 
{
	return hasProgress; 
}

double Button::getPercentComplete()
{ 
	return progress*100.0/total;
}

