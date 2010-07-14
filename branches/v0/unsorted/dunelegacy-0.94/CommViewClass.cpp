#include "CommViewClass.h"

#define ANIMMOVETIMER (frameLast - frameNow)+25


/*static*/ 
CommViewContent CommViewClass::CommViewContents[UNKNOWNTYPE];

CommViewClass::CommViewClass()
	:active(false),
	 animFrame(-1)
{};


/*virtual*/ 
CommViewClass::~CommViewClass()
{
	if(content.graphic)
		SDL_FreeSurface(content.graphic);
};


CommViewClass* 
CommViewClass::getCommViewClass(CommViewType type)
{
	static bool loaded = false;
	
	if (!loaded) 
	{
		
		extern SDL_Rect gameBarPos;
		loaded = true;

		/*initialize Content*/
		CommViewContents[AnimationRadarOn].itemID = Picture_RadarStatic;
		CommViewContents[AnimationRadarOn].animFrames  = 12;
		CommViewContents[AnimationRadarOn].isAnimating = 1;
		CommViewContents[AnimationRadarOn].animateBackwards = true;
		CommViewContents[AnimationRadarOn].noRepeat = true;
		                              /*don't know the right offset*/
		CommViewContents[AnimationRadarOn].position.x=gameBarPos.x + 15;
		CommViewContents[AnimationRadarOn].position.y=gameBarPos.y + 1;

		CommViewContents[AnimationRadarOff]=
			CommViewContents[AnimationRadarOn];

		CommViewContents[AnimationRadarOff].animateBackwards = false;
	};

if (type!=UNKNOWNTYPE)
{
	/*create new empty CommView*/
	CommViewClass* CommView = new CommViewClass();
		
	/*assign content*/
	CommView->content = CommViewContents[type];

	/*load graphics*/
	int itemID = CommView->content.itemID;
	
	CommView->content.graphic = (SDL_Surface*)(dataFile[itemID].dat);
	CommView->content.graphic = copySurface(CommView->content.graphic);
	CommView->active = true;
	return CommView;
}
else
	return 0;
};

void CommViewClass::draw()
{
	static int frameLast = 0;
	static int frameNow  = 0;
	static SDL_Rect source;	
	
	if (!active)
	  return;
	
	frameLast = frameNow;
	frameNow = SDL_GetTicks();
	int imageW,imageH;
	content.position.w = content.graphic->w;
	content.position.h = content.graphic->h;
	
	/*need to parametrize 4 & 3*/
	imageW = content.graphic->w/4;
	imageH = content.graphic->h/3;
    source.w = imageW;
    source.h = imageH;

	if (content.isAnimating)
		{
			static int animationTimer = 0;
		    //static int animFrame=-1;
		
			if((++animationTimer >= ANIMMOVETIMER) && content.isAnimating)
			
			{
				if(++animFrame >= content.animFrames)
				{
				 animFrame = 0;
				 source.x = 0;
				 source.y = 0;
				
				 //if no Repeat finish animation
				 if(content.noRepeat)
					{
					 active = false;
					 return;
					};

				};
				
				
				animationTimer = 0;
			
			/*get right picture*/
			if(content.animateBackwards)
			{
				source.x = content.graphic->w - imageW * (1+((animFrame) % 4));
				if (animFrame % 4 == 0)
					source.y = content.graphic->h- imageH * (1+((animFrame) / 4));	
			
			}
			else
			{
				source.x = imageW * (animFrame % 4);
				if (animFrame % 4 == 0)
					source.y = imageH * (animFrame / 4);
			};
		};		
				
	
		/*draw image*/
		if (animFrame > -1) 
			SDL_BlitSurface(content.graphic, &source, screen, &content.position);
		
};
};

