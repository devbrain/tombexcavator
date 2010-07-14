class Window : public Container
{
public:
	Window();
	~Window();

	void draw();
	void setSurface(SDL_Surface* newSurface, SDL_Rect* src);
private:
	SDL_Surface *surface;
	SDL_Rect	surfaceSrc;
};

