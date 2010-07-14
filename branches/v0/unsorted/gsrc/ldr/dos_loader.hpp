#ifndef __DOS_LOADER_HPP__
#define __DOS_LOADER_HPP__

#include <ostream>

class dos_exe_c
{
public:
	static dos_exe_c load (const char* filename);
public:
	unsigned short pages                () const;
	unsigned short bytes_of_last_sector () const; 
	unsigned short relocation_items     () const;
	unsigned short paragraphs_of_header () const; 
	unsigned short min_memory           () const;
	unsigned short max_memory           () const;
	unsigned short stack_offset         () const;
    unsigned short original_sp          () const;
	unsigned short chk_sum              () const;
	unsigned short original_ip          () const;
	unsigned short cs_offset_from_end   () const;
	unsigned short fst_relocation_item  () const;
	unsigned short overlay              () const;

	unsigned int   image_size   () const;
	unsigned int   image_offset () const; 

	friend std::ostream& operator << (std::ostream& os, const dos_exe_c& dos_exe);
private:
	dos_exe_c (); // disallow default constructor
private:
	unsigned short m_signature;            // MZ/ZM,sign of vaild .EXE file
	unsigned short m_pages;                // Pages of .EXE file,512B per page,always +1
	unsigned short m_bytes_of_last_sector; // Bytes of last sector
	unsigned short m_relocation_items;     // Items of relocation table
	unsigned short m_paragraphs_of_header; // Paragraphs of header
	unsigned short m_min_memory;           // Memory required at least
	unsigned short m_max_memory;           // Memory required at most
	unsigned short m_stack_offset;         // Stack offset from end the header
    unsigned short m_original_sp;		   // SP original value
	unsigned short m_chk_sum;			   // Checksum
	unsigned short m_original_ip;		   // IP original value
	unsigned short m_cs_offset_from_end;   // Code Segment offset from end the header
	unsigned short m_fst_relocation_item;  // Offset of the first relocation item
	unsigned short m_overlay;			   // Overlay number

	unsigned int m_image_size;
	unsigned int m_image_offset;
};


#endif
