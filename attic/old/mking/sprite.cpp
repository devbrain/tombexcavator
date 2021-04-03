#include "sprite.h"
#include "data_source.h"
// ---------------------------------------------------------
scan_line_c::~scan_line_c ()
{
	if (m_data)
	{
		delete [] m_data;
	}
	m_data = 0;
}
// ---------------------------------------------------------
const char* scan_line_c::data () const
{
	return m_data;
}
// ---------------------------------------------------------
unsigned int scan_line_c::start_x () const
{
	return m_start_x;
}
// ---------------------------------------------------------
unsigned int scan_line_c::width () const
{
	return m_width;
}
// ---------------------------------------------------------
scan_line_c::scan_line_c ()
:	m_start_x (0),
	m_width	  (0),
	m_data    (0)
{
	
}
// ---------------------------------------------------------
void scan_line_c::_init (unsigned int start_x, unsigned int width)
{
	m_start_x = start_x;
	m_width   = width;
	m_data    = new char [width];
}
// ---------------------------------------------------------
char* scan_line_c::_data ()
{
	return m_data;
}
// ==========================================================
sprite_c::sprite_c  ()
:	m_width (0),
	m_height (0),
	m_scanlines (0)
{
}
// ----------------------------------------------------------
sprite_c::~sprite_c ()
{
	if (m_scanlines)
	{
		delete [] m_scanlines;
	}
	m_scanlines = 0;
}
// ----------------------------------------------------------
void sprite_c::load (const ds::fat_entry_c* fe)
{
	data_source_c d (fe);
	d.read (m_width);
	d.read (m_height);
	unsigned int code;
	d.read (code);

	unsigned int* offsets = new unsigned int [m_height];
	d.read (offsets, m_height);
	m_scanlines = new scan_line_c [m_height];

	for (unsigned int y=0; y<m_height; y++)
	{
		unsigned int offset = offsets [y];
		if (offset)
		{
			d.seek (offset);
			int x_pos,y_pos,w;
			d.read (x_pos);
			d.read (y_pos);
			d.read (w);
			m_scanlines [y_pos]._init (x_pos, w);
			d.read (m_scanlines [y_pos].data (), w);
		}
	}
	delete [] offsets;
}
// ----------------------------------------------------------
unsigned int sprite_c::width  () const
{
	return m_width;
}
// ----------------------------------------------------------
unsigned int sprite_c::height () const
{
	return m_height;
}
// ----------------------------------------------------------
void sprite_c::blit(SDL_Surface* screen, int xpos, int ypos)
{
	for (unsigned int y=0; y<m_height; y++)
	{
		const scan_line_c& sc = m_scanlines [y];

		if (sc.width () > 0)
		{
			Uint8 *bufp = (Uint8*)screen->pixels + (ypos + y)*screen->pitch + xpos + sc.start_x ();
			memcpy (bufp, sc.data (), sc.width ());
		}
	}
	 SDL_UpdateRect(screen, xpos, ypos, m_width, m_height);
}