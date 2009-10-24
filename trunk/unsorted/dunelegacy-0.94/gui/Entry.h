class Entry : public Widget
{
public:
	Entry();
	~Entry();

	void draw();
	void handleKeyPress(SDL_KeyboardEvent *key);
	void handleMotion(Uint32 x, Uint32 y);
	void handlePress(Uint32 x, Uint32 y);
	void handleRelease(Uint32 x, Uint32 y);
	void setCallBackCancel(void (*newCallback)());
	void setCallBackConfirm(void (*newCallback)());
	void setEditable(bool truth);
	void setFocused(bool truth);
	void setText(char* newText);
	bool textEquals(char* checkText);

	inline char* getText() { return text; }
private:

	bool	editable;
	char	text[MAX_LINE];
	int		textPos;

	void (*callbackCancel)();
	void (*callbackConfirm)();

	SDL_Surface* textSurface;
};

