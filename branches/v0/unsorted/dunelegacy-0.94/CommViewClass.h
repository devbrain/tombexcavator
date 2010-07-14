#ifndef DUNE_COMMVIEW_H
#define DUNE_COMMVIEW_H

#include "sand.h"
 
typedef enum
{AnimationRadarOn, AnimationRadarOff, UNKNOWNTYPE}
CommViewType
;

typedef struct{
	int itemID;
	int animFrames;
	int isAnimating;
	bool animateBackwards;
	bool noRepeat;
	
	SDL_Surface* graphic;
	SDL_Rect position;
}CommViewContent;


class CommViewClass
{
public:
	/*use this to get an instance*/
	static CommViewClass* getCommViewClass(CommViewType type);
	
	/*draw CommView*/
	void draw();

	/*if not active CommView can be deleted*/
	bool isActive(){return active;};
	
	virtual ~CommViewClass();
	
private: 
		CommViewContent content;
		/*stores presets*/
		static CommViewContent CommViewContents[];
		/*private constructor so you hava to use
		 *getCommView to get an Instance*/
		CommViewClass();
		
		/*if finished playing, active is true*/
		bool active;

		/*which picture has to be drawn?*/
		int animFrame;
};
#endif //#define DUNE_COMMVIEW_H

