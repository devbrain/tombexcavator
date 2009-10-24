#include "../sand.h"
//#include "gui.h"

Label::Label() : Widget()
{
//	for (int i = 0; i < MAX_LINE; i++)
//		text[i] = ' ';

	nLines = 0;
	text = (char **)malloc(sizeof(char *) * nLines);

	textSurface = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * nLines);

	transparent = false;

//	text[0] = (char *)malloc(MAX_LINE);
//	text[0][0] = '\0';
	textSurface = NULL;

	textColour = COLOUR_WHITE;
	backColour = COLOUR_BLACK;
	borderColour = COLOUR_BLACK;

	justification = 0;
}
Label::~Label()
{
	for(int i=0;i<nLines;i++)
	{
		if (textSurface != NULL)
			SDL_FreeSurface(textSurface[i]);
	}
}

void Label::draw()
{
	if (visible)
	{
		SDL_Rect dest;

		if(!transparent)
		{
			SDL_FillRect(screen, &rect, backColour);
			drawrect(screen, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h, borderColour);
		}

		int ysum = 0;
		for(int i=0;i<nLines;i++)
		{
			if (textSurface[i] != NULL)
			{
				switch(justification)
				{
					case 0:
						dest = rect;
						dest.x = (rect.w - textSurface[i]->w)/2 + rect.x;
						dest.y = (rect.h - textSurface[i]->h)/2 + rect.y + ysum;
						break;

					case 1:
						dest = rect;
						dest.x = rect.x;
						dest.y = rect.y + ysum;
						break;

					case 2:
						dest = rect;
						dest.x = (rect.w - textSurface[i]->w)/2 + rect.x;
						dest.y = (rect.h - textSurface[i]->h)/2 + rect.y + ysum;
						//unimplemented
						break;
				}

				dest.w = textSurface[i]->w;
				dest.h = textSurface[i]->h;
				SDL_BlitSurface(textSurface[i], NULL, screen, &dest);

				ysum += dest.h + 5;
			}
		}
	}
}

void Label::handleMotion(Uint32 x, Uint32 y)
{

}

void Label::handlePress(Uint32 x, Uint32 y)
{

}

void Label::handleRelease(Uint32 x, Uint32 y)
{

}

void Label::setText(char *newText)
{
	//strncpy(text, newText, MAX_LINE);

	for(int i=0;i<nLines;i++)
	{
		if(textSurface[i] != NULL)
			SDL_FreeSurface(textSurface[i]);

		free(text[i]);
	}

	text = splitLines(newText, (int)'\n');

//	if (textSurface != NULL)
//		SDL_FreeSurface(textSurface);

	if(nLines > 0)
	{
		textSurface = (SDL_Surface **)malloc(sizeof(SDL_Surface *) * nLines);

		for(int i=0;i<nLines;i++)
		{
			textSurface[i] = TTF_RenderText_Solid(font[12], text[i], palette->colors[textColour]);
		}

		if(textSurface[0])
		{
			this->setHeight(textSurface[0]->h);
			this->setWidth(textSurface[0]->w);
		}
	}
}

char **Label::splitLines(char *text, int delim)
{
	char **newText;
	char *lastPtr;
	char *newPtr;
	int nTokens = 0;
	int len = 0;

	lastPtr = text;

	while(newPtr = strchr(lastPtr, delim))
	{
		nTokens++;		
		lastPtr = newPtr + 1;
	}

	newText = (char **)malloc(sizeof(char *) * nTokens);
	lastPtr = text;
	nTokens = 0;

	while(newPtr = strchr(lastPtr, delim))
	{
		len = newPtr - lastPtr;
		newText[nTokens] = (char *)malloc(MAX_LINE);
		memset(newText[nTokens], 0, MAX_LINE);
		strncpy(newText[nTokens], lastPtr, len);
		
		lastPtr = newPtr + 1;
		//newPtr += len;
		nTokens++;
	}

	if(nTokens == 0 && strlen(text) > 0)
	{
		nTokens = 1;
		//there is one, but it is not terminated with a \n
		newText = (char **)malloc(sizeof(char *) * nTokens);
		newText[0] = (char *)malloc(MAX_LINE);
		memset(newText[0], 0, MAX_LINE);
		strcpy(newText[0], text);
	}

	nLines = nTokens;
	//text = newText;
	return newText;
}

