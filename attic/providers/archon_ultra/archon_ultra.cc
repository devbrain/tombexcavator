//
// Created by igor on 26/05/2020.
//

#include "archon_ultra.hh"
#include <iostream>
#include "../../archive/io.hh"

namespace archon_ultra
{
    struct group_descriptor
    {
        group_descriptor(archive::input& inp)
        {
            inp >> num_of_types >> num_of_blocks >> disposition >> offset_to_first_block;
            offset_to_first_block.data &= 0x00FFFFFF;
        }
        uint8_t num_of_types;
        uint8_t num_of_blocks;
        archive::le_uint16_t disposition; // 0 - ARCHON.PKG 0x20 TITLE.PKG
        archive::le_uint32_t offset_to_first_block;
    };

    pkg_file::pkg_file(const std::string& path)
    : m_source(path)
    {
        archive::inmem_input input(reinterpret_cast<const unsigned char*>(m_source.data()), m_source.size());

        archive::le_uint16_t magic;
        archive::le_uint32_t length_block_start;
        archive::le_uint32_t length_block_end;
        archive::le_uint32_t magic2;
        archive::le_uint16_t grp_blocks_num;
        archive::le_uint16_t magic3;
        archive::le_uint32_t header_size;

        input >> magic >> length_block_start >> length_block_end >> magic2 >> grp_blocks_num >> magic3 >> header_size;

        if (magic.data != 0xFFFF || magic3.data != 3)
        {
            throw std::runtime_error("Invalid PKG file");
        }

        std::vector<group_descriptor> grp_descriptors;
        for (uint16_t i=0; i<grp_blocks_num.data; i++)
        {
            grp_descriptors.push_back(group_descriptor(input));
            if (grp_descriptors.back().num_of_types != 1)
            {
                std::cout << i << std::endl;
            }
        }

        std::cout << "zopa";
    }
} // ns archon_ultra
