class Label : public Widget
{
public:
	Label();
	~Label();

	void draw();
	void handleMotion(Uint32 x, Uint32 y);
	void handlePress(Uint32 x, Uint32 y);
	void handleRelease(Uint32 x, Uint32 y);
	void setText(char* newText);

//	inline char* getText() { return text; }

	inline void setTextColour(int i) { textColour = i; }
	inline void setBackgroundColour(int i) { backColour = i; }
	inline void setBorderColour(int i) { borderColour = i; }
	inline void setTransparent(bool i) { transparent = i; }
	inline void setJustification(int i) { justification = i; }
private:
	char **splitLines(char* text, int delim);

	char **text;
	int nLines;

	SDL_Surface** textSurface;

	int textColour, backColour, borderColour;

	bool	transparent;
	int		justification;//0 = centre, 1 = left, 2 = right
};

