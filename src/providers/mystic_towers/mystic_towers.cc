//
// Created by igor on 25/05/2020.
//
#include <iostream>
#include "mystic_towers.hh"
#include <archive/io.hh>

namespace mystic
{
    fat_entry::fat_entry (std::uint32_t offset_, std::uint32_t size_)
    : offset(offset_),
      size(size_)
    {

    }
    // ---------------------------------------------------------------------------------
    dat_archive::dat_archive(const std::string& path)
    : m_source(path)
    {
        archive::inmem_input input(reinterpret_cast<const unsigned char*>(m_source.data()), m_source.size());
        auto total_length = input.bytes_remains();
        input.seek(total_length - 2);
        archive::le_uint16_t num_files;
        input >> num_files;
        auto fat_start = 2 + num_files.data * 21;
        input.seek(total_length - fat_start);
        for (uint16_t i=0; i<num_files.data; i++)
        {
            uint8_t file_len;
            input >> file_len;
            char sname[12];
            input.read_buff(sname, 12);
            std::string name(sname, sname + file_len);
            archive::le_uint32_t offset, size;
            input >> offset >> size;
            m_fat.insert(std::make_pair(name, fat_entry(offset.data, size.data)));
        }

    }
    // ---------------------------------------------------------------------
    const std::map<std::string, fat_entry>& dat_archive::fat () const
    {
        return m_fat;
    }
}
