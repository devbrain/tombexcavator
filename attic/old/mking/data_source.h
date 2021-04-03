#ifndef __DATA_SOURCE_H__
#define __DATA_SOURCE_H__

#include <stddef.h>
#include <string.h>

namespace ds
{
	class fat_entry_c;
}

class data_source_c
{
public:
	data_source_c (const ds::fat_entry_c* fe);
	~data_source_c ();

	template <typename T>
	bool read (T& v)
	{
		if (m_pos + sizeof (T) > m_size)
		{
			return false;
		}

		memcpy ((char*)&v, m_data + m_pos, sizeof (T));
		m_pos += sizeof (v);
		return true;
	}
	
	template <typename T>
	bool read (T* v, size_t length)
	{
		if (m_pos + length*sizeof (T) > m_size)
		{
			return false;
		}
		memcpy ((char*)v, m_data + m_pos, sizeof (T)*length);
		m_pos += sizeof (v)*length;
		return true;
	}

	bool seek (size_t pos);
	
	size_t tell () const;
	size_t size () const;
	
private:
	char* m_data;
	size_t m_size;
	size_t m_pos;
};


#endif
