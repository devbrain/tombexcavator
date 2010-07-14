#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <SDL.h>

namespace ds
{
	class	fat_entry_c;
}

class sprite_c;
// -------------------------------------------------------------------
class scan_line_c
{
	friend class sprite_c;
public:
	~scan_line_c ();
	const char* data () const;

	unsigned int start_x () const;
	unsigned int width () const;
private:
	scan_line_c ();
	void _init (unsigned int start_x, unsigned int width);
	char* _data ();
private:
	scan_line_c (const scan_line_c&);
	scan_line_c& operator = (const scan_line_c&);
private:
	unsigned int m_start_x;
	unsigned int m_width;
	char*        m_data;
};
// ----------------------------------------------------------------
class sprite_c
{
public:
	sprite_c  ();
	~sprite_c ();
	void load (const ds::fat_entry_c* fe);
	unsigned int width  () const;
	unsigned int height () const;

	void blit (SDL_Surface *screen, int x, int y);
private:
	unsigned int m_width;
	unsigned int m_height;
	scan_line_c* m_scanlines;
};


#endif
