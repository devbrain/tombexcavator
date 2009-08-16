#ifndef ARCHIVE_H_INCLUDED
#define ARCHIVE_H_INCLUDED

#include "types.h"
#include "memarea.h"
#include "memarray.h"

#include <map>
#include <string>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>
#include <string.h>



class chaos_archive_c
{
    public:
        chaos_archive_c  (const std::string& path);

        off_t              fat_size     ()                        const;
        unsigned int       size         ()                        const;
        const std::string& entry_name   (unsigned int id)         const;
        unsigned int       entry_id     (const std::string& name) const;
        off_t              entry_offset (unsigned int id)         const;
        off_t              entry_size   (unsigned int id)         const;
        void               extract      (const std::string& path,
                                         unsigned int id)         const;

        void simul ();
     private:
        types::byte _read_1024   ();
        void        _build_table ();
        void        _decode ();
        void        _decode2 ();
    private:
        const size_t c_chunk_size;
        char*        m_chunk_buff;
        char*        m_chunk_ptr;
        size_t       m_has_bytes;

        memarea_c  m_table;

        memarray_c<types::word> m_arr_A;
        memarray_c<types::word> m_arr_Z;
        memarray_c<types::word> m_arr_D;
        memarray_c<types::word> m_arr_X;

     private:
        struct entry_descriptor_s
        {
            off_t       m_offset;
            off_t       m_size;
            std::string m_name;
        };

        typedef std::vector <entry_descriptor_s>     entry_vector_t;

        typedef std::map <std::string, unsigned int> name_id_map_t;
        typedef name_id_map_t::const_iterator        const_name_itr_t;
        typedef name_id_map_t::iterator              name_itr_t;
        typedef name_id_map_t::value_type            name_val_t;

        name_id_map_t  m_names_map;
        entry_vector_t m_entries;
        off_t          m_fat_size;

        mutable std::ifstream m_chaos_ifs;
    private:
        chaos_archive_c (const chaos_archive_c&);
        const chaos_archive_c& operator = (const chaos_archive_c&);

};

#endif // ARCHIVE_H_INCLUDED
