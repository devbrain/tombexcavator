#ifndef DUNE_WIDGET_H
#define DUNE_WIDGET_H

class Widget

{
public:
	Widget();
	~Widget();



	virtual void draw() = 0;

	virtual void setFocused(bool truth);

	virtual void handleCancelPress(Uint32 x, Uint32 y);

	virtual void handleCancelRelease(Uint32 x, Uint32 y);

	virtual void handleMotion(Uint32 x, Uint32 y) = 0;

	virtual void handlePress(Uint32 x, Uint32 y);

	virtual void handleRelease(Uint32 x, Uint32 y) = 0;



	void setWidth(Uint32 newWidth);

	void setHeight(Uint32 newHeight);



	inline bool isABox() { return aBox; }

	inline bool isAnEntry() { return anEntry; }

	inline bool isEnabled() { return enabled; }

	inline void setEnabled(bool newState) { enabled = newState; }

	inline void setVisible(bool newState) { visible = newState; }

	inline void setX(Uint32 newX) { rect.x = newX; }

	inline void setY(Uint32 newY) { rect.y = newY; }

	inline Uint32 getX() { return rect.x; }

	inline Uint32 getY() { return rect.y; }

	inline Uint32 getWidth() { return rect.w; }

	inline Uint32 getHeight() { return rect.h; }

protected:

	bool		aBox,

				anEntry,

				enabled,

				visible;

	SDL_Rect	rect;
};
#endif //#define DUNE_WIDGET_H

