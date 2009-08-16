#include "data_source.h"
#include "ds_volume.h"

// ---------------------------------------------------------------------
data_source_c::data_source_c (const ds::fat_entry_c* fe)
{
	m_size = fe->length ();
	m_data = new char [m_size];
	fe->read (m_data);
	m_pos = 0;
}
// ---------------------------------------------------------------------
data_source_c::~data_source_c ()
{
	delete [] m_data;
}
// ---------------------------------------------------------------------
bool data_source_c::seek (size_t pos)
{
	if (pos >= m_size)
	{
		return false;
	}
	m_pos = pos;
	return true;
}
// ---------------------------------------------------------------------
size_t data_source_c::tell () const
{
	return m_pos;
}
// ---------------------------------------------------------------------
size_t data_source_c::size () const
{
	return m_size;
}
