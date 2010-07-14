class Button : public Widget

{

public:

	Button();

	~Button();



	void draw();

	void handleCancelPress(Uint32 x, Uint32 y);

	void handleCancelRelease(Uint32 x, Uint32 y);

	void handleMotion(Uint32 x, Uint32 y);

	void handlePress(Uint32 x, Uint32 y);

	void handleRelease(Uint32 x, Uint32 y);

	void setSurface(SDL_Surface* newSurface, SDL_Rect* src);

	void setPressedSurface(SDL_Surface* newSurface, SDL_Rect* src);

	void setHelpText(char* newText);

	void setText(char* newText);

	void setToggled(bool toggle);

	bool textEquals(char* checkText);

	char* getText();



	int getProgress();

	void setProgress(int i);

	void setTotalProgress(int i);

	void setIsProgress(bool i);

	bool isProgress();

	double getPercentComplete();



	inline void setTextColour(int i) { textColour = i; }

	inline void setBackgroundColour(int i) { backGroundColour = i; }

	inline void setPressedTextColour(int i) { pressedTextColour = i; }

	inline void setBorderColour(int i) { borderColour = i; }

	inline void setTransparent(bool i) { transparent = i; }



	void setCallBack(void (*newCallback)());

	void setCallBack(void (*newCallback)(void* data), void* newData);

	void setCallBackCancel(void (*newCallback)());

	void setCallBackCancel(void (*newCallback)(void* data), void* newData);

	

	inline void setToggler(bool toggle) { toggler = toggle; }

	inline bool hasHelpText() { return (strlen(helpText) > 0); }





private:

	void (*callbackClick)();

	void (*callbackClick2)(void* data);

	void (*callbackCancel)();

	void (*callbackCancel2)(void* data);

	void *callBackData;

	void *callBackCancelData;



	bool	mouseOver, pressed, toggled, toggler;

	char	helpText[MAX_LINE],

			text[MAX_LINE];

	Uint16	mouseLocationX, mouseLocationY;

	SDL_Rect	surfaceSrc,

				pressedSurfaceSrc;

	SDL_Surface	*surface,

				*pressedSurface,

				*textSurface,

				*helpTextSurface;



	bool hasProgress, transparent;

	int total, progress;

	int textColour, backGroundColour;

	int pressedTextColour, borderColour;

};

