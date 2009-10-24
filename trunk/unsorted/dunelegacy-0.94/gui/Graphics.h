void catch_stdin();

void clearMessage();

void mapImageHouseColour(SDL_Surface* graphic, int house);

void mapImageHouseColourBase(SDL_Surface* graphic, int house, int baseCol);

void putpixel(SDL_Surface *surface, int x, int y, Uint32 colour);

Uint32 getpixel(SDL_Surface *surface, int x, int y);

void hlineNoLock(SDL_Surface *surface, int x1, int y, int x2, Uint32 colour);

void drawhline(SDL_Surface *surface, int x1, int y, int x2, Uint32 colour);

void drawrect(SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint32 colour);

void showMessage(char* newMessage);

SDL_Surface* copySurface(SDL_Surface* inSurface);

SDL_Surface* scaleSurface(SDL_Surface *surf, double ratio);

