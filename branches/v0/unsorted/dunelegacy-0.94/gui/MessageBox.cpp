#include "../sand.h"

#define MESSAGETIME 495
#define SLOWDOWN timer/55

extern SDL_Surface *MessageBoxSurface;

MessageBox::MessageBox()
:setup(false),
 timer(-MESSAGETIME/2)
{}
;

void MessageBox::addMessage(std::string const& msg)
{
	bool found = false;
	
	/*if message is already there, do nothing*/
	std::queue<std::string> msgcpy(Messages);
	while(msgcpy.size())
	{
		if(msgcpy.front() == msg)
		 found = true;
	    msgcpy.pop();
	};
	
	if(!found)	
	 Messages.push(msg);
};

void MessageBox::addUrgentMessage(std::string const& msg)
{
	while(Messages.size())
		Messages.pop();

	Messages.push(msg);
};

void MessageBox::initialize()
{
	myButton.setSurface(MessageBoxSurface, NULL);
	myButton.setHeight(MessageBoxSurface->h);
	myButton.setWidth(MessageBoxSurface->w);
	myButton.setCallBack(NULL, NULL);
	myButton.setVisible(true);
	setup = true;
};

void MessageBox::draw()
{
	if(!setup)
		initialize();
	
	//draw background
	myButton.draw();
	
	if(Messages.size()>0)
	{
	if(timer++ == (MESSAGETIME/3))
	{
		timer = -MESSAGETIME/2;
		//delete first message
		if(Messages.size())
			Messages.pop();
		
		/*if no more messages leave*/
		if(!Messages.size())
		{
			timer = -MESSAGETIME/2;
			return;
		};
	};
	
	

	
		//draw text
		SDL_Rect textLocation;
		textLocation.x = getX()+16;
		textLocation.y = getY()+9;
		if (timer>0)
			textLocation.y -= SLOWDOWN;

		SDL_Surface *surface = TTF_RenderText_Solid(font[8], Messages.front().c_str(), palette->colors[COLOUR_BLACK]);
		//drawLocation.x = 0;
		//drawLocation.y = TTF_FontHeight(font[16])*pos++;
	
		SDL_Rect cut;
		cut.x = 0;
		cut.y = 0;
		
		if (timer>0)
			cut.y = 3*SLOWDOWN;
		
		cut.h = surface->h - cut.y;
		cut.w = surface->w;
		SDL_BlitSurface(surface, &cut, screen, &textLocation);
		SDL_FreeSurface(surface);
	};
};

