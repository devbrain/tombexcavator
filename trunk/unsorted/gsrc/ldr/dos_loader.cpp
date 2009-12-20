#include "dos_loader.hpp"

#include <fstream>

dos_exe_c dos_exe_c::load (const char* filename)
{
	std::ifstream ifs (filename, std::ios::binary);

	dos_exe_c result;

	ifs.read ((char*) &result.m_signature           , 2);
	ifs.read ((char*) &result.m_bytes_of_last_sector, 2);
	ifs.read ((char*) &result.m_pages               , 2);
	ifs.read ((char*) &result.m_relocation_items    , 2);
	ifs.read ((char*) &result.m_paragraphs_of_header, 2);
	ifs.read ((char*) &result.m_min_memory          , 2);
	ifs.read ((char*) &result.m_max_memory          , 2);
	ifs.read ((char*) &result.m_stack_offset        , 2);
    ifs.read ((char*) &result.m_original_sp         , 2);
	ifs.read ((char*) &result.m_chk_sum             , 2);
	ifs.read ((char*) &result.m_original_ip         , 2);
	ifs.read ((char*) &result.m_cs_offset_from_end  , 2);
	ifs.read ((char*) &result.m_fst_relocation_item , 2);
	ifs.read ((char*) &result.m_overlay             , 2);


	result.m_image_offset = 16 * result.m_paragraphs_of_header;

	if (result.m_pages > 0)
	{
		result.m_image_size = (512 * (result.m_pages - 1)) + result.m_bytes_of_last_sector;
	}
	else
	{
		result.m_image_size = result.m_bytes_of_last_sector;
	}

	result.m_image_size -= result.m_image_offset;

	return result;
}

dos_exe_c::dos_exe_c ()
{
}

// =================================================================
std::ostream& operator << (std::ostream& os, const dos_exe_c& dos_exe)
{
	os  << std::hex
		<< "MZ/ZM,sign of vaild .EXE file:             : 0x" << dos_exe.m_signature            << std::endl
		<< std::dec
		<< "Pages of .EXE file,512B per page,always +1 : " << dos_exe.m_pages                << std::endl
		<< "Bytes of last sector                       : " << dos_exe.m_bytes_of_last_sector << std::endl
		<< "Items of relocation table                  : " << dos_exe.m_relocation_items     << std::endl
		<< "Paragraphs of header                       : " << dos_exe.m_paragraphs_of_header << std::endl
		<< "Memory required at least                   : " << dos_exe.m_min_memory           << std::endl
		<< "Memory required at most                    : " << dos_exe.m_max_memory           << std::endl
		<< "Stack offset from end the header           : " << dos_exe.m_stack_offset         << std::endl
		<< "SP original value                          : " << dos_exe.m_original_sp          << std::endl
		<< "Checksum                                   : " << dos_exe.m_chk_sum              << std::endl
		<< "IP original value                          : " << dos_exe.m_original_ip          << std::endl
		<< "Code Segment offset from end the header    : " << dos_exe.m_cs_offset_from_end   << std::endl
		<< "Offset of the first relocation item        : " << dos_exe.m_fst_relocation_item  << std::endl
		<< "Overlay number                             : " << dos_exe.m_overlay              << std::endl
		<< "Image offset                               : " << dos_exe.m_image_offset         << std::endl
		<< "Image size                                 : " << dos_exe.m_image_size;

	 return os;
}

// =================================================================
// Getters
// =================================================================
unsigned short dos_exe_c::pages                () const
{
	return m_pages;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::bytes_of_last_sector () const
{
	return m_bytes_of_last_sector;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::relocation_items     () const
{
	return m_relocation_items;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::paragraphs_of_header () const
{
	return m_paragraphs_of_header;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::min_memory           () const
{
	return m_min_memory;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::max_memory           () const
{
	return m_max_memory;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::stack_offset         () const
{
	return m_stack_offset;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::original_sp          () const
{
	return m_original_sp;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::chk_sum              () const
{
	return m_chk_sum;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::original_ip          () const
{
	return m_original_ip;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::cs_offset_from_end   () const
{
	return m_cs_offset_from_end;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::fst_relocation_item  () const
{
	return m_fst_relocation_item;
}
// ----------------------------------------------------------------
unsigned short dos_exe_c::overlay              () const
{
	return m_overlay;
}
// ----------------------------------------------------------------
unsigned int dos_exe_c::image_size   () const
{
	return m_image_size;
}
// ----------------------------------------------------------------
unsigned int dos_exe_c::image_offset () const
{
	return m_image_offset;
}