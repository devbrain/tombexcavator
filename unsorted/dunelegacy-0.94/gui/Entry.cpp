#include "../sand.h"
//#include "gui.h"
extern Widget *focusedWidget;

Entry::Entry() : Widget()
{
	anEntry = true;
	editable = true;

	callbackCancel = callbackConfirm = NULL;

	text[0] = '\0';
	textSurface = NULL;
}
Entry::~Entry()
{
	if (textSurface != NULL)
		SDL_FreeSurface(textSurface);
}

void Entry::draw()
{
	if (visible)
	{
		SDL_Rect dest;

		if (focusedWidget == this)
		{
			SDL_FillRect(screen, &rect, COLOUR_BLACK);
			drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, COLOUR_WHITE);
		}
		else
		{
			SDL_FillRect(screen, &rect, COLOUR_WHITE);
			drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, COLOUR_BLACK);
		}

		if (textSurface != NULL)
		{
			dest = rect;
			dest.x = (rect.w - textSurface->w)/2 + rect.x;
			dest.y = (rect.h - textSurface->h)/2 + rect.y;
			dest.w = textSurface->w;
			dest.h = textSurface->h;
			SDL_BlitSurface(textSurface, NULL, screen, &dest);
		}
	}
}

void Entry::setFocused(bool truth)
{
	Widget::setFocused(truth);

	if (textSurface != NULL)
		SDL_FreeSurface(textSurface);
	if (truth)
		textSurface = TTF_RenderText_Solid(font[12], text, palette->colors[COLOUR_WHITE]);
	else
		textSurface = TTF_RenderText_Solid(font[12], text, palette->colors[COLOUR_BLACK]);
}

void Entry::handleKeyPress(SDL_KeyboardEvent* key)
{
	if (key->keysym.sym == SDLK_ESCAPE)
	{
		if (callbackCancel)
			callbackCancel();

		setFocused(false);
	}
	else if (key->keysym.sym == SDLK_RETURN)
	{
		if (callbackConfirm)
			callbackConfirm();

		setFocused(false);
	}
	else if (editable)
	{
		if (key->keysym.sym == SDLK_BACKSPACE)
		{
			if (--textPos < 0)
				textPos = 0;
			text[textPos] = '\0';

			setFocused(true);
		}
		else if (textPos < MAX_NAMELENGTH-1)
		{
			if ((key->keysym.unicode < 0x80) && (key->keysym.unicode > 0))
			{
				text[textPos++] = (char)key->keysym.unicode;
				setFocused(true);
			}
		}
	}
}

void Entry::handleMotion(Uint32 x, Uint32 y)
{

}

void Entry::handlePress(Uint32 x, Uint32 y)
{
	if (enabled
		&& (((x == NONE) && (y == NONE))
			|| (((x >= getX()) && (x < (getX() + getWidth())))
				&& ((y >= getY()) && (y < (getY() + getHeight()))))))
	{
		Widget::handlePress(x, y);

		setFocused(true);
		textPos = (int)strlen(text);
	}
}

void Entry::handleRelease(Uint32 x, Uint32 y)
{
}

void Entry::setCallBackCancel(void (*newCallback)())
{
	callbackCancel = newCallback;
}

void Entry::setCallBackConfirm(void (*newCallback)())
{
	callbackConfirm = newCallback;
}

void Entry::setEditable(bool truth)
{
	editable = truth;
}

void Entry::setText(char *newText)
{
	strncpy(text, newText, MAX_NAMELENGTH);
	setFocused(false);
}

bool Entry::textEquals(char* checkText)
{
	return (strcmp(checkText, text) == 0);
}

