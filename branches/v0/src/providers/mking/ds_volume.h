#ifndef __DS_VOLUME_H__
#define __DS_VOLUME_H__

#include "broken-compiler.h"

#include <string>
#include <map>

namespace ds
{
	class volume_c;

	class fat_entry_c
	{
		friend class volume_c;
	public:
		enum type_t
		{
			sprite,
			raw,
			unknown,
			no_type
		};
	public:
		std::string name () const;
		size_t length () const;

		void read (char* buffer) const;
		type_t type () const;
	private:
		fat_entry_c (const std::string& path_to_volume, const std::string& name, 
					 size_t offset, size_t length, type_t type = unknown);
	private:
		const std::string m_path;
		const std::string m_name;
		const size_t      m_offset;
		const size_t      m_length;
		const type_t      m_type;
	};
	// ------------------------------------------------------------------------------------
	class volume_c
	{
	public:
		volume_c (const char* path);
		const fat_entry_c* get (const std::string& name) const;
	protected:
	//	virtual fat_entry_c::type_t _get_type (const char* name);
	private:
	//	void _build_fat (const char* path);
	private:
		typedef std::map <std::string, fat_entry_c*> fat_t;
		typedef fat_t::iterator                      iterator_t;
		typedef fat_t::const_iterator                const_iterator_t;
		typedef fat_t::value_type                    val_t;

		fat_t m_fat;
	};
}


#endif
