#include "broken-compiler.h"
#include "ds_volume.h"

#include <iostream>
#include <fstream>

namespace ds
{
	template <typename T>
    size_t ifs_read (std::ifstream& ifs, T& data)
    {
        ifs.read (reinterpret_cast <char*> (&data), sizeof (T));
        return sizeof (T);
    }
	// =========================================================================

	std::string fat_entry_c::name () const
	{
		return m_name;
	}
	// --------------------------------------------------------------------------------------
	size_t fat_entry_c::length () const
	{
		return m_length;
	}
	// --------------------------------------------------------------------------------------
	void fat_entry_c::read (char* buffer) const
	{
		std::ifstream ifs;

		ifs.open (m_path.c_str (), std::ios::binary | std::ios::in);

		ifs.seekg (m_offset);
		ifs.read (buffer, m_length);
	}
	// --------------------------------------------------------------------------------------
	fat_entry_c::type_t fat_entry_c::type () const
	{
		return m_type;
	}
	// --------------------------------------------------------------------------------------
	fat_entry_c::fat_entry_c (const std::string& path_to_volume, const std::string& name, 
							  size_t offset, size_t length, type_t type)
							  : m_path (path_to_volume),
							  m_name (name),
							  m_offset (offset),
							  m_length (length),
							  m_type (type)
	{
	}
	// ========================================================================================
	volume_c::volume_c (const char* path)
	{
		std::ifstream ifs;

		ifs.open (path, std::ios::binary | std::ios::in);
		unsigned char magic [8] = { 0xDE, 0xAD, 0xBE, 0xEF,
						   0xDE, 0xAD, 0xBE, 0xEF };
		ifs.read ((char*)magic, 8);
		unsigned int entries_num;
		
		ifs_read (ifs, entries_num);
		for (unsigned int i=0; i< entries_num; i++)
		{
			unsigned int unk;
			unsigned int offset;
			unsigned int size;
			char name [16];
			ifs_read (ifs, unk);
			ifs_read (ifs, offset);
			ifs_read (ifs, size);
			ifs.read (name, 16);
			
			m_fat.insert (val_t(std::string (name), new fat_entry_c (path, name, offset, size)));
			//std::cout << name << " : " << size << std::endl;
		}
	}
	// =========================================================================================
	const fat_entry_c* volume_c::get (const std::string& name) const
	{
		const_iterator_t i = m_fat.find (name);
		if (i == m_fat.end ())
		{
			return 0;
		}
		return i->second;
	}
}
